#include "DCMImageSeries.h"
#include "gdcmImageReader.h"
#include "gdcmAttribute.h"
#include "gdcmTag.h"
#include "gdcmScanner.h"
#include "gdcmIPPSorter.h"
#include "gl/Util.h"

using namespace std;
using namespace gdcm;

template <typename T>
void applyModalityLUT(T* buffer, int width, int height, int depth, double slope, double intercept)
{
    int totalPixels = width * height * depth;
    for (int i = 0; i < totalPixels; i++) {
        *buffer = (*buffer) * slope + intercept;
        buffer++;
    }
}


DCMImageSeries::DCMImageSeries(int width, int height, int depth, GLenum format, GLenum type)
    : VolumeData(width, height, depth, format, type)
{
}

vector<DCMImageSeries::ID> DCMImageSeries::search(const char* directoryPath)
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
            }
        }
    }
    
    return ids;
}

vector<string> DCMImageSeries::sortFiles(DCMImageSeries::ID id)
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
    sorter.SetZSpacingTolerance(0.000001);
    
    // weak error checking
    if (!sorter.Sort(unsorted)) {
        for (string s : unsorted)
            std::cout << s << endl;
        cerr << "Problem sorting images!" << endl;
    }
    return sorter.GetFilenames();
}

DCMImageSeries* DCMImageSeries::load(const char* directoryPath)
{
    std::vector<ID> ids = search(directoryPath);
    return (ids.size() == 0 ? NULL : load(ids[0]));
}

DCMImageSeries* DCMImageSeries::load(DCMImageSeries::ID id)
{
    vector<string> files = sortFiles(id);
    if (files.size() < 2)
        return NULL;
    
    // using the first image to read dimensions that shouldn't change
    ImageReader reader;
    reader.SetFileName(files[0].c_str());
    reader.Read();
    Image& img = reader.GetImage();
    DataSet& dataSet = reader.GetFile().GetDataSet();
    int width = img.GetColumns();
    int height = img.GetRows();
    int depth = files.size();
    
    // only supporting 8/16-bit monochrome images (CT and MR)
    // even if CT data is stored unsigned, it is always signed after the
    // conversion to Hounsfield units (might need to worry about how I do
    // this if the source data has more than 12 out of the 16 bytes used)
    GLenum format = GL_RED;
    GLenum type;
    switch (img.GetPixelFormat())
    {
        case PixelFormat::INT8:
            type = GL_BYTE;
            break;
        case PixelFormat::UINT8:
            type = (id.modality == CT) ? GL_BYTE : GL_UNSIGNED_BYTE;
            break;
        case PixelFormat::INT16:
            type = GL_SHORT;
            break;
        case PixelFormat::UINT16:
            type = (id.modality == CT) ? GL_SHORT : GL_UNSIGNED_SHORT;
            break;
        default:
            return NULL;
            break;
    }
    
    // load all the images into the series
    DCMImageSeries* series = new DCMImageSeries(width, height, depth, format, type);
    img.GetBuffer(series->data);
    gl::flipImage(series->data, series->width, series->height, series->getPixelSize());
    for (int i = 1; i < series->depth; i++) {
        size_t offset = i * series->getImageSize();
        ImageReader reader;
        reader.SetFileName(files[i].c_str());
        reader.Read();
        reader.GetImage().GetBuffer(series->data + offset);
        gl::flipImage(series->data + offset, series->width, series->height, series->getPixelSize());
        cout << (double)i / series->depth << std::endl;
    }
    
    // transform from manufacturer values to modality values
    double intercept = img.GetIntercept();
    double slope = img.GetSlope();
    switch (type)
    {
        case GL_BYTE:
            applyModalityLUT((GLbyte*)series->data, width, height, depth, slope, intercept);
            break;
        case GL_UNSIGNED_BYTE:
            applyModalityLUT((GLubyte*)series->data, width, height, depth, slope, intercept);
            break;
        case GL_SHORT:
            applyModalityLUT((GLshort*)series->data, width, height, depth, slope, intercept);
            break;
        case GL_UNSIGNED_SHORT:
            applyModalityLUT((GLushort*)series->data, width, height, depth, slope, intercept);
            break;
        default:
            return NULL;
            break;
    }
    
    // store value of interest LUTs as windows
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
        
        // convert to normalized windows
        
        
        series->windows.push_back(Window(centers[i], widths[i]));
    }
    delete[] centers;
    delete[] widths;
    
    return series;
}
