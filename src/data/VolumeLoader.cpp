#include "VolumeLoader.h"
#include "gdcmImageReader.h"
#include "gdcmAttribute.h"
#include "gdcmTag.h"
#include "gdcmScanner.h"
#include "gdcmIPPSorter.h"
#include "util/Util.h"
#include <thread>
#include <regex>

#if defined(_WIN32)
#include "util/dirent.h"
#define DELIM "\\"
#else
#define DELIM "/"
#include <dirent.h>
#endif

extern "C" {
#include "tiffio.h"
}

using namespace std;
using namespace gdcm;
using namespace gl;

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
	else if (source.type == Source::TIFF_DIR) {
		//TODO load logic, get directory of images.
		if (state == READY) {

			thread t(&VolumeLoader::loadTIFF, this, source.name);
			t.detach();
		}
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

std::string VolumeLoader::getStateMessage() const
{
    return stateMessage;
}

void VolumeLoader::loadRAW(const std::string& fileName)
{
	auto work = [=] {
		string datName = fileName.substr(0, fileName.size() - 4) + ".txt";
		ifstream f(datName);
		string line;

		if (f.is_open()) {
			this->state = LOADING;
			stateMessage = "Reading RAW";

			this->volume = new VolumeData;

			getline(f, line);

			smatch matches;
			regex_match(line, matches, regex{"\\D*(\\d+)x(\\d+)x(\\d+)"});
			volume->width = std::stoi(matches[1]);
			volume->height = std::stoi(matches[2]);
			volume->depth = std::stoi(matches[3]);

			getline(f, line);
			regex_match(line, matches, regex{ ("\\D*(\\d+)") });
			unsigned pixelBytes = std::stoi(matches[1]);

			getline(f, line);
			regex_match(line, matches, regex{ ("scale: (.*):(.*):(.*)") });
			float x, y, z;
			x = stof(matches[1]);
			y = stof(matches[2]);
			z = stof(matches[3]);

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

			this->state = FINISHED;
			stateMessage = "Finished";
		}
	};

	thread t(work);
	t.detach();
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
    //img.GetBuffer(volume->data);
    //gl::flipImage(volume->data, volume->width, volume->height, volume->getPixelSizeBytes());
    
    // load all other images
  //  for (int i = 0; i < volume->depth; i++) {
  //      size_t offset = i * volume->getSliceSizeBytes();
  //      ImageReader reader;
  //      reader.SetFileName(files[i].c_str());
  //      reader.Read();
  //      reader.GetImage().GetBuffer(volume->data + offset);
		//gl::flipImage(volume->data + offset, volume->width, volume->height, volume->getPixelSizeBytes());
  //  }

	for (int i = 0; i < volume->depth; i++) {
		size_t offset = (volume->depth - i - 1) * volume->getSliceSizeBytes();
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
			Interval intvl;

			switch (volume->type)
			{
			case GL_BYTE:
				intvl.center(static_cast<GLbyte>(centers[i]));
				intvl.width(static_cast<GLbyte>(widths[i]));
				break;
			case GL_UNSIGNED_BYTE:
				intvl.center(static_cast<GLubyte>(centers[i]));
				intvl.width(static_cast<GLubyte>(widths[i]));
				break;
			case GL_SHORT:
				intvl.center(static_cast<GLshort>(centers[i]));
				intvl.width(static_cast<GLshort>(widths[i]));
				break;
			case GL_UNSIGNED_SHORT:
				intvl.center(static_cast<GLushort>(centers[i]));
				intvl.width(static_cast<GLushort>(widths[i]));
				break;
			}

			volume->windows_.push_back(intvl);
        }
        delete[] centers;
        delete[] widths;
        
        // patient orientation
        const double* cosines = img.GetDirectionCosines();
		Vec3 x(static_cast<float>(cosines[0]), static_cast<float>(cosines[1]), static_cast<float>(cosines[2]));
		Vec3 y(static_cast<float>(cosines[3]), static_cast<float>(cosines[4]), static_cast<float>(cosines[5]));
		Vec3 z = x.cross(y);
	
        volume->orientation = Mat3(x, y, z);
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

void VolumeLoader::sortTIFF(const std::string& directoryPath, std::vector<string>& files) {

	DIR* dir = opendir(directoryPath.c_str());

	struct dirent* entry = readdir(dir);

	while (entry != NULL)
	{
		std::string name{ entry->d_name };
		if ((name.size() > 3 && name.substr(name.size() - 4, 4) == ".tif") ||
			(name.size() > 4 && name.substr(name.size() - 5, 5) == ".tiff")) {
			files.push_back(directoryPath + DELIM + name);
		}
		entry = readdir(dir);
	}


	//sort lexocographically
	if (false) {
		std::sort(files.begin(), files.end());
	}
	else {
		std::sort(files.begin(), files.end(), [](string a, string b) {
			int x = a.find_last_of('\\');
			int off = a.find('.') - x - 1;
			string a_ = a.substr(x + 1, off);
			x = b.find_last_of('\\');
			off = b.find('.') - x - 1;
			string b_ = b.substr(x + 1, off);
			int i = atoi(a_.c_str());
			int j = atoi(b_.c_str());
			return i < j;
		});
	}


}

void VolumeLoader::loadTIFF(std::string& directoryPath)
{

	int iPrev = _CrtSetReportMode(_CRT_ASSERT, 0);

	this->state = LOADING;

	// sort TIFF files so they are ordered correctly along Z
	stateMessage = "Scanning TIFF files";
	vector<string> files;
	double zSpacing;

	sortTIFF(directoryPath, files);

	if (files.size() < 2) {
		this->volume = NULL;
		return;
	}

	// using the first image to read dimensions that shouldn't change
	cout << files[0].c_str() << endl;

	wstring fname = wstring(files[0].begin(), files[0].end());

	//TIFF* img = TIFFOpenW(fname.c_str(), "r");
	TIFF* img = TIFFOpen(files[0].c_str(), "r");

	stateMessage = "Opened First tif";


	if (!img) {
		cout << "image not opened properly" << endl;
		this->volume = NULL;
		return;
	}

	volume = new VolumeData;
	volume->modality = VolumeData::Modality::UNKNOWN;
	TIFFGetField(img, TIFFTAG_IMAGEWIDTH, &(volume->width));
	TIFFGetField(img, TIFFTAG_IMAGELENGTH, &(volume->height));
	volume->depth = static_cast<unsigned int>(files.size());

	uint32 tw, th;
	TIFFGetField(img, TIFFTAG_TILEWIDTH, &tw);
	TIFFGetField(img, TIFFTAG_TILELENGTH, &th);

	cout << "tw: " << tw << " th: " << th << endl;


	cout << "read width: " << volume->width << " and height: " << volume->height << endl;

	volume->format = GL_RED;
	uint16 format;
	uint16 bits_per;
	TIFFGetField(img, TIFFTAG_SAMPLEFORMAT, &format);
	TIFFGetField(img, TIFFTAG_BITSPERSAMPLE, &bits_per);

	cout << "read bits and format" << endl;

	switch (format)
	{
	case SAMPLEFORMAT_UINT:
		switch (bits_per) {
		case 8:
			volume->type = GL_UNSIGNED_BYTE;
			break;
		case 16:
			volume->type = GL_UNSIGNED_SHORT;
			break;
		case 32:
			volume->type = GL_UNSIGNED_INT;
			break;
		case 64:
			volume->type = GL_UNSIGNED_INT64_NV;
			break;
		}
		break;
	case SAMPLEFORMAT_INT:
		switch (bits_per) {
		case 8:
			volume->type = GL_BYTE;
			break;
		case 16:
			volume->type = GL_SHORT;
			break;
		case 32:
			volume->type = GL_INT;
			break;
		case 64:
			volume->type = GL_INT64_NV;
			break;
		}
		break;
	case SAMPLEFORMAT_IEEEFP:
		switch (bits_per) {
		case 32:
			volume->type = GL_FLOAT;
			break;
		case 64:
			volume->type = GL_DOUBLE;
			break;
		}
		break;
	case SAMPLEFORMAT_VOID:
	default:
		switch (bits_per) {
		case 8:
			volume->type = GL_UNSIGNED_BYTE;
			break;
		case 16:
			volume->type = GL_UNSIGNED_SHORT;
			break;
		case 32:
			volume->type = GL_UNSIGNED_INT;
			break;
		case 64:
			volume->type = GL_UNSIGNED_INT64_NV;
			break;
		}
		break;

	}

	volume->getPixelSizeBytes();

	cout << bits_per << " bits per pixel" << endl;

	cout << "about to load tif directory" << endl;

	// now that the type and dimensions are known, allocate memory for voxels
	stateMessage = "Reading TIFF Images";
	volume->data = new char[volume->width * volume->height * volume->depth * gl::sizeOf(volume->type)];

	// load first image (already in reader memory)
	//img.GetBuffer(volume->data);
	//gl::flipImage(volume->data, volume->width, volume->height, volume->getPixelSizeBytes());

	// load all other images
	//  for (int i = 0; i < volume->depth; i++) {
	//      size_t offset = i * volume->getSliceSizeBytes();
	//      ImageReader reader;0
	//      reader.SetFileName(files[i].c_str());
	//      reader.Read();
	//      reader.GetImage().GetBuffer(volume->data + offset);
	//gl::flipImage(volume->data + offset, volume->width, volume->height, volume->getPixelSizeBytes());
	//  }

	for (int i = 0; i < volume->depth; i++) {

		size_t offset = (volume->depth - i - 1) * volume->getSliceSizeBytes();

		img = TIFFOpen(files[i].c_str(), "r");

		//cout << files[i] << endl;

		//cout << "offset: " << offset << endl;

		//cout << i << ": stripsize: " << TIFFStripSize(img) << " #strips: " << TIFFNumberOfStrips(img) << endl;

		/*
		tsize_t acc = 0;
		for (int row = 0; row < volume->height; row++) {
			tsize_t r;
			r = TIFFReadScanline(img, volume->data + (offset + acc), row);
			if (r == -1) {
				cout << "error reading tiff" << endl;
			}
			acc += r;
		}
		*/


		
		tstrip_t strip;
		tsize_t acc = 0;

		for (strip = 0; strip < TIFFNumberOfStrips(img); strip++) {
			//cout << "strip: " << strip << endl;
			tsize_t r = TIFFReadRawStrip(img, strip, volume->data + (offset + acc), (tsize_t)-1);
			if (r == -1) {
				cout << "error reading tiff" << endl;
			}
			acc += r;
		}
		
		

		//cout << "read " << acc << " bytes" << endl;

		TIFFClose(img);
	}


	/*
	for (int i = 0; i < volume->depth; i++) {
		cout << "i: " << i;
		size_t offset = (volume->depth - i - 1) * volume->getSliceSizeBytes();
		img = TIFFOpen(files[i].c_str(), "r");

		cout << "line size: " << TIFFScanlineSize(img) << " # cols" << volume->width << endl;

		cout << "j: ";

		for (int j = 0; j < volume->height; j++) {
			cout << j << " ";
			size_t row_offset = j * (TIFFScanlineSize(img));
			TIFFReadScanline(img, volume->data + offset + row_offset, j);
		}
		cout << endl;

		gl::flipImage(volume->data + offset, volume->width, volume->height, volume->getPixelSizeBytes());
	}
	*/


	//TODO use the correct tiff attributes
	// Z spacing should be regular between images (this is NOT slice thickness attribute)
	{
		volume->setVoxelSize(1, 1, 1);
	}

	cout << "after loading tiffs" << endl;
	
	// Apply modality LUT (if possible) and update min/max values
	switch (volume->type)
	{
	case GL_BYTE:
			calculateMinMax<GLbyte>();
		stateMessage = "Calculating Gradients";
		volume->computeGradients<GLbyte>();
		break;
	case GL_UNSIGNED_BYTE:
			calculateMinMax<GLubyte>();
		stateMessage = "Calculating Gradients";
		volume->computeGradients<GLubyte>();
		break;
	case GL_SHORT:
			calculateMinMax<GLshort>();
		stateMessage = "Calculating Gradients";
		volume->computeGradients<GLshort>();
		break;
	case GL_UNSIGNED_SHORT:
			calculateMinMax<GLushort>();
		stateMessage = "Calculating Gradients";
		volume->computeGradients<GLushort>();
		break;
	case GL_INT:
		calculateMinMax<GLint>();
		stateMessage = "Calculating Gradients";
		volume->computeGradients<GLint>();
		break;
	case GL_UNSIGNED_INT:
		calculateMinMax<GLuint>();
		stateMessage = "Calculating Gradients";
		volume->computeGradients<GLuint>();
		break;
	case GL_INT64_NV:
		calculateMinMax<GLint64>();
		stateMessage = "Calculating Gradients";
		volume->computeGradients<GLint64>();
		break;
	case GL_FLOAT:
		calculateMinMax<GLfloat>();
		stateMessage = "Calculating Gradients";
		volume->computeGradients<GLfloat>();
		break;
	case GL_DOUBLE:
		calculateMinMax<GLdouble>();
		stateMessage = "Calculating Gradients";
		volume->computeGradients<GLdouble>();
		break;
	default:
		break; // should not happen
	}
	
	// store value of interest LUTs as windows
	stateMessage = "Calculating Gradients";

	cout << "about to get name" << endl;

	{
		//char name[500];
		//TIFFGetField(img, TIFFTAG_DOCUMENTNAME, name);
		//volume->name = name;
		volume->name = files[0].c_str();
	}

	cout << "Finished" << endl;

	state = FINISHED;
	stateMessage = "Finished";

	_CrtSetReportMode(_CRT_ASSERT, iPrev);
}

VolumeData* VolumeLoader::getVolume()
{
    VolumeData* result = volume;
    volume = NULL;
    state = READY;
    stateMessage = "Idle";
    return result;
}
