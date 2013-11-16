#ifndef __MEDLEAP_DCM_IMAGE_SERIES__
#define __MEDLEAP_DCM_IMAGE_SERIES__

#include "VolumeData.h"
#include <string>

class DCMImageSeries : public VolumeData
{
public:
    
    /** The type of image data */
    enum Modality
    {
        CT,
        MR
    };
    
    /** Identifies a DICOM image series on disk */
    struct ID
    {
        std::string uid;
        std::string directory;
        Modality modality;
        unsigned int numImages;
    };
    
    /** This will search a directory to find all unique CT or MT image series. */
    static std::vector<ID> search(const char* directoryPath);
    
    /** Loads the specified image series */
    static DCMImageSeries* load(ID seriesID);
    
    /** This will load the first image series found in a directory */
    static DCMImageSeries* load(const char* directoryPath);
    
    /** Returns all of the file names associated with an image series, sorted by Z */
    static std::vector<std::string> sortFiles(ID seriesID);
private:
    Modality modality;
    
    /** Can only construct by using static methods */
    DCMImageSeries(int width, int height, int depth, GLenum format, GLenum type);
};

#endif // __MEDLEAP_DCM_IMAGE_SERIES__