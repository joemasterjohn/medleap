#include "DCMImporter.h"
#include "gdcmImageReader.h"
#include "gdcmAttribute.h"
#include "gdcmTag.h"
#include "gdcmDirectoryHelper.h"
#include "gdcmScanner.h"
#include "gdcmIPPSorter.h"

using namespace std;
using namespace gdcm;

vector<string> sortImages(DCMImporter::SeriesID series);
void flipImage(char* buffer, int pixelSize, int width, int height);

DCMImporter::DCMImporter(){}

DCMImporter::~DCMImporter(){}

std::vector<DCMImporter::SeriesID> DCMImporter::findSeriesIDs(const char* directory)
{
    vector<SeriesID> ids;
    Directory::FilenamesType series;
    
    series = DirectoryHelper::GetMRImageSeriesUIDs(directory);
    for (string id : series) {
        SeriesID seriesID = { id, "MR", directory };
        ids.push_back(seriesID);
    }
    
    series = DirectoryHelper::GetCTImageSeriesUIDs(directory);
    for (string id : series) {
        SeriesID seriesID = { id, "CT", directory };
        ids.push_back(seriesID);
    }
    
    return ids;
}

VolumeData* DCMImporter::importVolume(const char* directory)
{
    vector<SeriesID> series = findSeriesIDs(directory);
    return importVolume(series[0]);
}

VolumeData* DCMImporter::importVolume(DCMImporter::SeriesID series)
{
    vector<string> files = sortImages(series);
    if (files.size() == 0)
        return NULL;
    
    ImageReader reader;
    reader.SetFileName(files[0].c_str());
    reader.Read();
    
    Image& img = reader.GetImage();
    DataSet& dataSet = reader.GetFile().GetDataSet();
    
    int width = img.GetColumns();
    int height = img.GetRows();
    int depth = files.size();
    double intercept = img.GetIntercept();
    double slope = img.GetSlope();
    GLenum type;
    int bytesPerPixel;
    switch (img.GetPixelFormat())
    {
        case PixelFormat::INT8:
            bytesPerPixel = 1;
            type = GL_BYTE;
            break;
        case PixelFormat::INT16:
            bytesPerPixel = 2;
            type = GL_SHORT;
            break;
        case PixelFormat::UINT8:
            bytesPerPixel = 1;
            type = GL_UNSIGNED_BYTE;
            break;
        case PixelFormat::UINT16:
            bytesPerPixel = 2;
            type = GL_UNSIGNED_SHORT;
            break;
        default:
            return NULL;
            break;
    }
    
    std::cout << img.GetPixelFormat() << std::endl;

    double windowCenter;
    {
        Attribute<0x0028,0x1050> a;
        a.Set(dataSet);
        windowCenter = a.GetValue();
    }
    
    double windowWidth;
    {
        Attribute<0x0028,0x1051> a;
        a.Set(dataSet);
        windowWidth = a.GetValue();
    }
    
    int imageSize = width * height * bytesPerPixel;
    char* buffer = new char[imageSize * depth];
    
    // load all the images into a single buffer
    img.GetBuffer(buffer);
    flipImage(buffer, bytesPerPixel, width, height);
    for (int i = 1; i < depth; i++) {
        ImageReader reader;
        reader.SetFileName(files[i].c_str());
        reader.Read();
        reader.GetImage().GetBuffer(buffer + i * imageSize);
        flipImage(buffer + i * imageSize, bytesPerPixel, width, height);
    }
    
    // apply modality LUT to convert manufacturer-dependent values to modality units
    // assuming UINT16!!! WARNING
    double inputMin = windowCenter - windowWidth / 2.0;
    double inputMax = windowCenter + windowWidth / 2.0;
    unsigned short* pixel = (unsigned short*)buffer;
    for (int i = 0; i < width * height * depth; i++) {
        unsigned short realWorldValue = (*pixel) * slope + intercept;
        
        // need to do the window too I guess.. (temporary)
        
        // VOI LUT (temporary: put in shader)
        if (realWorldValue <= inputMin)
            realWorldValue = 0;
        else if (realWorldValue >= inputMax)
            realWorldValue = 65535;
        else {
        double normalized = (double)(realWorldValue - inputMin) / (inputMax - inputMin);
            realWorldValue = normalized * 65535;
        }
        *pixel = realWorldValue;
        pixel++;
    }
    
    // CT or MR should always be monochrome, so format is always GL_RED
    return new VolumeData((unsigned char*)buffer, width, height, depth, GL_RED, type);
}

/**
 * Returns a vector of file names for all the DICOM images in a series. The file names
 * are ordered by increasing Z.
 */
vector<string> sortImages(DCMImporter::SeriesID series)
{
    // use directory from the series
    Directory directory;
    directory.Load(series.directory);
    
    // scan directory by series UID
    Tag uid(0x0020,0x000e);
    Scanner scanner;
    scanner.AddTag(uid);
    scanner.Scan(directory.GetFilenames());
    
    // find the matching series UID
    Directory::FilenamesType uids = scanner.GetOrderedValues(uid);
    for (string id : uids) {
        if (id == series.id) {
            // get all associated DICOM files
            Directory::FilenamesType files = scanner.GetAllFilenamesFromTagToValue(uid, id.c_str());
            
            // sort files by Z (tolerance is default from GDCM sample code)
            IPPSorter sorter;
            sorter.SetComputeZSpacing(true);
            sorter.SetZSpacingTolerance(0.000001);
            
            // weak error checking
            if (!sorter.Sort(files))
                cerr << "Problem sorting images!" << endl;
            
            return sorter.GetFilenames();
        }
    }
    
    // the series wasn't found for some reason
    return vector<string>();
}

/**
 * Flips an image vertically so the pixel data is read bottom to top (OpenGL style)
 */
void flipImage(char* buffer, int pixelSize, int width, int height)
{
    int rowSize = pixelSize * width;
    char temp[rowSize];
    
    // reverse rows int image
    for (int i = 0; i < height/2; i++) {
        memcpy(temp, buffer + i * rowSize, rowSize);
        memcpy(buffer + i * rowSize, buffer + (height - 1 - i) * rowSize, rowSize);
        memcpy(buffer + (height - 1 - i) * rowSize, temp, rowSize);
    }
}