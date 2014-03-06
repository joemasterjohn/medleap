#include "VolumeData.h"
#include "util/Util.h"
#include "gdcmImageReader.h"
#include "gdcmAttribute.h"
#include "gdcmTag.h"
#include "gdcmScanner.h"
#include "gdcmIPPSorter.h"
#include "volume/Histogram.h"

using namespace gdcm;
using namespace std;

VolumeData::VolumeData()
{
    width = 0;
    height = 0;
    depth = 0;
    format = 0;
    type = 0;
    minVoxelValue = 0;
    maxVoxelValue = 0;
    voxelSize.x = voxelSize.y = voxelSize.z = 0;
    bounds = new BoundingBox(1,1,1);
    modality = UNKNOWN;
    activeWindow = 0;
    histogram = 0;
}

VolumeData::~VolumeData()
{
    if (histogram)
        delete histogram;
}

void VolumeData::loadTexture2D(GLuint &texture, int depth)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat(), width, height, 0, format, type, data + depth * getImageSize());
}

void VolumeData::loadTexture3D(cgl::Texture* texture)
{
    texture->bind();
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat(), width, height, depth, 0, format, type, data);
}

void VolumeData::setVoxelSize(float x, float y, float z)
{
    this->voxelSize.x = x;
    this->voxelSize.y = y;
    this->voxelSize.z = z;
    
    float totalWidth = x * width;
    float totalHeight = y * height;
    float totalDepth = z * depth;
    
    cgl::Vec3 v(totalWidth, totalHeight, totalDepth);
    v.normalize();
    delete bounds;
    bounds = new BoundingBox(v.x, v.y, v.z);
}

const BoundingBox& VolumeData::getBounds()
{
    return *bounds;
}

const cgl::Vec3& VolumeData::getVoxelSize() const
{
    return voxelSize;
}

unsigned int VolumeData::getWidth()
{
    return width;
}

unsigned int VolumeData::getHeight()
{
    return height;
}

unsigned int VolumeData::getDepth()
{
    return depth;
}

unsigned int VolumeData::getImageSize()
{
    return getPixelSize() * width * height;
}

unsigned int VolumeData::getPixelSize()
{
    return gl::sizeOf(type);
}

unsigned int VolumeData::getVolumeSize()
{
    return getImageSize() * depth;
}

unsigned int VolumeData::getNumVoxels()
{
    return width * height * depth;
}

int VolumeData::getMinValue()
{
    return minVoxelValue;
}

int VolumeData::getMaxValue()
{
    return maxVoxelValue;
}

GLenum VolumeData::getType()
{
    return type;
}

GLenum VolumeData::internalFormat()
{
    if (format == GL_RED) {
        if (type == GL_BYTE)
            return GL_R8_SNORM;
        if (type == GL_SHORT)
            return GL_R16_SNORM;
        return GL_RED;
    }
    
    // for now I shouldn't be using multi-channel data, but
    // if I do let's use the same as data buffer format
    return format;
}

bool VolumeData::isSigned()
{
    return type == GL_BYTE || type == GL_SHORT;
}

VolumeData::Modality VolumeData::getModality()
{
    return modality;
}

Histogram* VolumeData::getHistogram()
{
    return histogram;
}

const cgl::Mat3& VolumeData::getPatientBasis() const
{
    return orientation;
}

vector<Window>& VolumeData::getWindows()
{
    return windows;
}

bool VolumeData::hasValue(uint16_t G, uint16_t E)
{
    return reader.GetFile().GetDataSet().FindDataElement(Tag(G,E));
}

Window& VolumeData::getCurrentWindow()
{
    return windows[activeWindow];
}

void VolumeData::setNextWindow()
{
    activeWindow = (activeWindow + 1) % windows.size();
}

void VolumeData::setPrevWindow()
{
    if (activeWindow == 0)
        activeWindow = windows.size() - 1;
    else
        activeWindow = (activeWindow - 1) % windows.size();
}








vector<VolumeData::ID> VolumeData::Loader::search(const char* directoryPath)
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
                ID id = { seriesID, directoryPath, CT, files.size() };
                ids.push_back(id);
            } else if (strModality == "MR") {
                ID id = { seriesID, directoryPath, MR, files.size() };
                ids.push_back(id);
            } else {
                ID id = { seriesID, directoryPath, UNKNOWN, files.size() };
                ids.push_back(id);
            }
        }
    }
    
    return ids;
}

void VolumeData::Loader::sortFiles(VolumeData::ID id, vector<string>& fileNames, double* zSpacing)
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
        cerr << "Problem sorting images!" << endl;
    }
    
    fileNames = sorter.GetFilenames();
    *zSpacing = sorter.GetZSpacing();
}

VolumeData* VolumeData::Loader::load(const char* directoryPath)
{
    std::vector<ID> ids = search(directoryPath);
    return (ids.size() == 0 ? NULL : load(ids[0]));
}

VolumeData* VolumeData::Loader::load(VolumeData::ID id)
{
    // sort DCM files so they are ordered correctly along Z
    vector<string> files;
    double zSpacing;
    sortFiles(id, files, &zSpacing);
    if (files.size() < 2)
        return NULL;

    
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
    volume->depth = files.size();

    
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
            volume->type = (id.modality == CT) ? GL_BYTE : GL_UNSIGNED_BYTE;
            break;
        case PixelFormat::INT16:
            volume->type = GL_SHORT;
            break;
        case PixelFormat::UINT16:
            volume->type = (id.modality == CT) ? GL_SHORT : GL_UNSIGNED_SHORT;
            break;
        default:
            delete volume;
            return NULL;
            break;
    }

    // now that the type and dimensions are known, allocate memory for voxels
    volume->data = new char[volume->width * volume->height * volume->depth * gl::sizeOf(volume->type)];
    
    // load first image (already in reader memory)
    img.GetBuffer(volume->data);
    gl::flipImage(volume->data, volume->width, volume->height, volume->getPixelSize());
    
    // load all other images
    for (int i = 1; i < volume->depth; i++) {
        size_t offset = i * volume->getImageSize();
        ImageReader reader;
        reader.SetFileName(files[i].c_str());
        reader.Read();
        reader.GetImage().GetBuffer(volume->data + offset);
        gl::flipImage(volume->data + offset, volume->width, volume->height, volume->getPixelSize());
    }
    
    // transform from manufacturer values to modality values
    if (volume->modality != UNKNOWN) {
        double intercept = img.GetIntercept();
        double slope = img.GetSlope();
        
        // numBins in histogram is hard-coded to 1024 for now... might want this to be more robust
        switch (volume->type)
        {
            case GL_BYTE:
                applyModalityLUT<GLbyte>(slope, intercept);
                volume->histogram = new Histogram(volume->minVoxelValue, volume->maxVoxelValue, 1024);
                volume->histogram->readData((GLbyte*)volume->data, volume->getNumVoxels());
                break;
            case GL_UNSIGNED_BYTE:
                applyModalityLUT<GLubyte>(slope, intercept);
                volume->histogram = new Histogram(volume->minVoxelValue, volume->maxVoxelValue, 1024);
                volume->histogram->readData((GLubyte*)volume->data, volume->getNumVoxels());
                break;
            case GL_SHORT:
                applyModalityLUT<GLshort>(slope, intercept);
                volume->histogram = new Histogram(volume->minVoxelValue, volume->maxVoxelValue, 1024);
                volume->histogram->readData((GLshort*)volume->data, volume->getNumVoxels());
                break;
            case GL_UNSIGNED_SHORT:
                applyModalityLUT<GLushort>(slope, intercept);
                volume->histogram = new Histogram(volume->minVoxelValue, volume->maxVoxelValue, 1024);
                volume->histogram->readData((GLushort*)volume->data, volume->getNumVoxels());
                break;
            default:
                return NULL;
                break;
        }
    }
    
    // store value of interest LUTs as windows
    if (volume->modality != UNKNOWN) {
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
            window.setReal(centers[i], widths[i]);
            volume->windows.push_back(window);
        }
        delete[] centers;
        delete[] widths;
        
        // patient orientation
        const double* cosines = img.GetDirectionCosines();
        cgl::Vec3 x(cosines[0], cosines[1], cosines[2]);
        cgl::Vec3 y(cosines[3], cosines[4], cosines[5]);
        cgl::Vec3 z = x.cross(y);
        volume->orientation = cgl::Mat3(x, y, z);
    } else {
        volume->windows.push_back(Window(volume->type));
    }
    
    // give the series a reader to the first image so it can get meta data
    volume->reader.SetFileName(files[0].c_str());
    volume->reader.Read();
    
    
    // Z spacing should be regular between images (this is NOT slice thickness attribute)
    const double* pixelSpacing = volume->getValues<const double*, 0x0028, 0x0030>();
    volume->setVoxelSize(pixelSpacing[0], pixelSpacing[1], zSpacing);
    
    return volume;
}