#ifndef __MEDLEAP_VOLUME_LOADER__
#define __MEDLEAP_VOLUME_LOADER__

#include "volume/VolumeData.h"
#include "gdcmReader.h"
#include "gdcmAttribute.h"

/** Utility class for constructing a VolumeData from DICOM image series */
class VolumeLoader
{
public:
    /** Identifies a DICOM image series on disk */
    struct ID
    {
        std::string uid;
        std::string directory;
        VolumeData::Modality modality;
        unsigned int numImages;
    };
    
    /** This will search a directory to find all unique CT or MT image series. */
    std::vector<ID> search(const char* directoryPath);
    
    /** Loads the specified image series */
    VolumeData* load(ID seriesID);
    
    /** This will load the first image series found in a directory */
    VolumeData* load(const char* directoryPath);
    
    /** Stores file names sorted by Z into the fileNames parameter. Also stores the computed Z spacing into zSpacing parameter. */
    void sortFiles(ID seriesID, std::vector<std::string>& fileNames, double* zSpacing);
    
private:
    VolumeData* volume;
    
    /** The modality LUT transforms device-dependent values to device-independent modality values. For example, it will transform raw UINT16 CT data values into signed CT Hounsfield units. It uses the slope and intercept stored in the DICOM dataset to transform values. */
    template <typename T>
    void applyModalityLUT(double slope, double intercept)
    {
        T* buffer = (T*)volume->data;
        volume->minVoxelValue = std::numeric_limits<int>::infinity();
        volume->maxVoxelValue = -std::numeric_limits<int>::infinity();
        for (int i = 0; i < volume->getNumVoxels(); i++) {
            *buffer = (*buffer) * slope + intercept;
            if (*buffer > volume->maxVoxelValue) volume->maxVoxelValue = *buffer;
            if (*buffer < volume->minVoxelValue) volume->minVoxelValue = *buffer;
            buffer++;
        }
    }
};

#endif // __MEDLEAP_VOLUME_LOADER__