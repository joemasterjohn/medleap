#include "VolumeLoader.h"
#include "gdcmImageReader.h"
#include "gdcmAttribute.h"
#include "gdcmTag.h"
#include "gdcmScanner.h"
#include "gdcmIPPSorter.h"
#include "util/Util.h"
#include <thread>

using namespace std;
using namespace gdcm;

VolumeLoader::VolumeLoader()
{
    volume = NULL;
    state = READY;
    stateMessage = "Idle";
}

vector<VolumeLoader::ID> VolumeLoader::search(const std::string& directoryPath)
{
    vector<ID> ids;
    
    // find all DICOM files in the directory that have a series UID
    Tag uid(0x0020,0x000e);
    Tag modality(0x0008,0x0060);
    Directory directory;
    directory.Load(directoryPath);
    Scanner scanner;
    scanner.AddTag(uid);
    scanner.AddTag(modality);
    scanner.Scan(directory.GetFilenames());
    vector<string> seriesIDs = scanner.GetOrderedValues(uid);
    
    // go through each series and check its type
    for (string seriesID : seriesIDs) {
        vector<string> files = scanner.GetAllFilenamesFromTagToValue(uid, seriesID.c_str());
        
        // a volume must have more than 1 image, so I'm ignoring other series
        if (files.size() > 1) {
            string strModality = scanner.GetValue(files[0].c_str(), modality);
            if (strModality == "CT") {
                ID id = { seriesID, directoryPath, VolumeData::CT, (unsigned)files.size() };
                ids.push_back(id);
            } else if (strModality == "MR") {
                ID id = { seriesID, directoryPath, VolumeData::MR, (unsigned)files.size() };
                ids.push_back(id);
            } else {
                ID id = { seriesID, directoryPath, VolumeData::UNKNOWN, (unsigned)files.size() };
                ids.push_back(id);
            }
        }
    }
    
    return ids;
}

void VolumeLoader::sortFiles(VolumeLoader::ID id, vector<string>& fileNames, double* zSpacing)
{
    // use directory from the series
    Directory directory;
    directory.Load(id.directory);
    
    // scan directory by series UID
    Tag uid(0x0020,0x000e);
    Scanner scanner;
    scanner.AddTag(uid);
    scanner.Scan(directory.GetFilenames());
    
    // sort files by (tolerance is default from GDCM sample code)
    Directory::FilenamesType unsorted = scanner.GetAllFilenamesFromTagToValue(uid, id.uid.c_str());
    
    IPPSorter sorter;
    sorter.SetComputeZSpacing(true);
    sorter.SetZSpacingTolerance(0.001);
    
    // weak error checking
    if (!sorter.Sort(unsorted)) {
        cerr << "Warning: could not sort using IPP/IOP." << endl;
		// use slicethickness or 1 and default order of files
		fileNames = directory.GetFilenames();
		*zSpacing = 1;
	} else {
		fileNames = sorter.GetFilenames();
		*zSpacing = sorter.GetZSpacing();
	}
}

void VolumeLoader::setSource(const Source& source)
{
	if (source.type == Source::DICOM_DIR) {
		std::vector<ID> ids = search(source.name);
		if (!ids.empty())
			setSource(ids[0]);
		else
			std::cout << "WARNING: directory does not seem to contain DICOM images" << std::endl;
	}
	else {
		loadRAW(source.name);
	}
}

void VolumeLoader::setSource(VolumeLoader::ID id)
{
    if (state == READY) {
        this->id = id;
        
        thread t(&VolumeLoader::load, this);
        t.detach();
    }
}

VolumeLoader::State VolumeLoader::getState()
{
    return state;
}

std::string VolumeLoader::getStateMessage()
{
    return stateMessage;
}

void VolumeLoader::loadRAW(const std::string& fileName)
{
	string datName = fileName.substr(0, fileName.size() - 4) + ".txt";
	ifstream f(datName);
	string line;

	if (f.is_open()) {
		this->state = LOADING;
		stateMessage = "Reading RAW";

		this->volume = new VolumeData;

		getline(f, line);
		volume->width = std::stoi(line);
		getline(f, line);
		volume->height = std::stoi(line);
		getline(f, line);
		volume->depth = std::stoi(line);

		getline(f, line);
		unsigned pixelBytes = std::stoi(line);

		getline(f, line);
		float x = std::stof(line);
		getline(f, line);
		float y = std::stof(line);
		getline(f, line);
		float z = std::stof(line);
		f.close();

		ifstream binary(fileName, ios::in | ios::binary);
		volume->data = new char[volume->getNumVoxels()*pixelBytes];
		binary.read(volume->data, volume->getNumVoxels()*pixelBytes);
		binary.close();

		volume->setVoxelSize(x, y, z);

		if (pixelBytes == 1) {
			volume->type = GL_UNSIGNED_BYTE;
			calculateMinMax<GLubyte>();
			volume->computeGradients<GLubyte>();
		}
		else {
			volume->type = GL_UNSIGNED_SHORT;
			calculateMinMax<GLushort>();
			volume->computeGradients<GLushort>();
		}
		volume->format = GL_RED;
		volume->name = fileName;
		volume->windows.push_back(Window(volume->type));

		this->state = FINISHED;
		stateMessage = "Finished";
	}
}

void VolumeLoader::load()
{
    this->state = LOADING;
    
    // sort DCM files so they are ordered correctly along Z
    stateMessage = "Scanning DICOM files";
    vector<string> files;
    double zSpacing;
    sortFiles(id, files, &zSpacing);
    if (files.size() < 2) {
        this->volume = NULL;
        return;
    }
    
    
    // using the first image to read dimensions that shouldn't change
    ImageReader reader;
    reader.SetFileName(files[0].c_str());
    reader.Read();
    Image& img = reader.GetImage();
    DataSet& dataSet = reader.GetFile().GetDataSet();
    
    volume = new VolumeData;
    volume->modality = id.modality;
    volume->width = img.GetColumns();
    volume->height = img.GetRows();
    volume->depth = static_cast<unsigned int>(files.size());
    
    
    // only supporting 8/16-bit monochrome images (CT and MR)
    // even if CT data is stored unsigned, it is always signed after the
    // conversion to Hounsfield units (might need to worry about how I do
    // this if the source data has more than 12 out of the 16 bytes used)
    volume->format = GL_RED;
    switch (img.GetPixelFormat())
    {
        case PixelFormat::INT8:
            volume->type = GL_BYTE;
            break;
        case PixelFormat::UINT8:
            volume->type = (id.modality == VolumeData::CT) ? GL_BYTE : GL_UNSIGNED_BYTE;
            break;
        case PixelFormat::INT16:
            volume->type = GL_SHORT;
            break;
        case PixelFormat::UINT16:
            volume->type = (id.modality == VolumeData::CT) ? GL_SHORT : GL_UNSIGNED_SHORT;
            break;
        default:
            delete volume;
            volume = NULL;
            return;
    }
    
    // now that the type and dimensions are known, allocate memory for voxels
    stateMessage = "Reading DICOM Images";
    volume->data = new char[volume->width * volume->height * volume->depth * gl::sizeOf(volume->type)];
    
    // load first image (already in reader memory)
    img.GetBuffer(volume->data);
    gl::flipImage(volume->data, volume->width, volume->height, volume->getPixelSizeBytes());
    
    // load all other images
    for (int i = 1; i < volume->depth; i++) {
        size_t offset = i * volume->getSliceSizeBytes();
        ImageReader reader;
        reader.SetFileName(files[i].c_str());
        reader.Read();
        reader.GetImage().GetBuffer(volume->data + offset);
		gl::flipImage(volume->data + offset, volume->width, volume->height, volume->getPixelSizeBytes());
    }

	// Z spacing should be regular between images (this is NOT slice thickness attribute)
	{
		Attribute<0x0028, 0x0030> at;
		at.SetFromDataSet(dataSet);
		const double* pixelSpacing = at.GetValues();
		volume->setVoxelSize(
			static_cast<float>(pixelSpacing[0]),
			static_cast<float>(pixelSpacing[1]),
			static_cast<float>(zSpacing));
	}
    
    // Apply modality LUT (if possible) and update min/max values
    switch (volume->type)
    {
        case GL_BYTE:
            if (volume->modality == VolumeData::UNKNOWN)
                calculateMinMax<GLbyte>();
            else
                applyModalityLUT<GLbyte>(img.GetSlope(), img.GetIntercept());
            stateMessage = "Calculating Gradients";
            volume->computeGradients<GLbyte>();
            break;
        case GL_UNSIGNED_BYTE:
            if (volume->modality == VolumeData::UNKNOWN)
                calculateMinMax<GLubyte>();
            else
                applyModalityLUT<GLubyte>(img.GetSlope(), img.GetIntercept());
            stateMessage = "Calculating Gradients";
            volume->computeGradients<GLubyte>();
            break;
        case GL_SHORT:
            if (volume->modality == VolumeData::UNKNOWN)
                calculateMinMax<GLshort>();
            else
                applyModalityLUT<GLshort>(img.GetSlope(), img.GetIntercept());
            stateMessage = "Calculating Gradients";
            volume->computeGradients<GLshort>();
            break;
        case GL_UNSIGNED_SHORT:
            if (volume->modality == VolumeData::UNKNOWN)
                calculateMinMax<GLushort>();
            else
                applyModalityLUT<GLushort>(img.GetSlope(), img.GetIntercept());
            stateMessage = "Calculating Gradients";
            volume->computeGradients<GLushort>();
            break;
        default:
            break; // should not happen
    }
    
    // store value of interest LUTs as windows
    stateMessage = "Calculating Gradients";
    if (volume->modality != VolumeData::UNKNOWN) {
        int numWindows;
        double* centers;
        double* widths;
        {
            Attribute<0x0028,0x1050> a;
            a.Set(dataSet);
            numWindows = a.GetNumberOfValues();
            centers = new double[numWindows];
            memcpy(centers, a.GetValues(), sizeof(double) * numWindows);
        }
        {
            Attribute<0x0028,0x1051> a;
            a.Set(dataSet);
            widths = new double[numWindows];
            memcpy(widths, a.GetValues(), sizeof(double) * numWindows);
        }
        for (int i = 0; i < numWindows; i++) {
            Window window(volume->type);
            window.setReal(static_cast<float>(centers[i]), static_cast<float>(widths[i]));
            volume->windows.push_back(window);
        }
        delete[] centers;
        delete[] widths;
        
        // patient orientation
        const double* cosines = img.GetDirectionCosines();
		Vec3 x(static_cast<float>(cosines[0]), static_cast<float>(cosines[1]), static_cast<float>(cosines[2]));
		Vec3 y(static_cast<float>(cosines[3]), static_cast<float>(cosines[4]), static_cast<float>(cosines[5]));
		Vec3 z = x.cross(y);
        volume->orientation = Mat3(x, y, z);
    } else {
        volume->windows.push_back(Window(volume->type));
    }


	{
		Attribute<0x0010, 0x0010> at;
		at.SetFromDataSet(dataSet);
		std::string name = at.GetValue();
		replace(name.begin(), name.end(), '^', ' ');
		volume->name = name;
	}
    
    state = FINISHED;
    stateMessage = "Finished";
}

VolumeData* VolumeLoader::getVolume()
{
    VolumeData* result = volume;
    volume = NULL;
    state = READY;
    stateMessage = "Idle";
    return result;
}
