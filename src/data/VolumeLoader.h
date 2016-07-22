#ifndef __MEDLEAP_VOLUME_LOADER__
#define __MEDLEAP_VOLUME_LOADER__

#include "VolumeData.h"
#include "gdcmReader.h"
#include "gdcmAttribute.h"



/** Utility class for constructing a VolumeData from DICOM image series */
class VolumeLoader
{
public:

	struct Source
	{
		enum Type { DICOM_DIR, RAW, TIFF_DIR };
		std::string name;
		Type type;
	};

    /** Identifies a DICOM image series on disk */
    struct ID
    {
        std::string uid;
        std::string directory;
        VolumeData::Modality modality;
        unsigned int numImages;
    };
    
    enum State
    {
        READY, LOADING, FINISHED
    };
    
    VolumeLoader();
    
    /** This will search a directory to find all unique CT or MT image series. */
    std::vector<ID> search(const std::string& directoryPath);
    
    /** Loads the specified image series. */
    void setSource(ID seriesID);
    
    /** This will load the first image series found in a directory */
    void setSource(const Source& source);
    
    /** Stores file names sorted by Z into the fileNames parameter. Also stores the computed Z spacing into zSpacing parameter. */
    void sortFiles(ID seriesID, std::vector<std::string>& fileNames, double* zSpacing);


	void sortTIFF(const std::string& directoryPath, std::vector<std::string>& files);

	void loadRAW(const std::string& fileName);

    /** Retrieves the previously loaded volume, or NULL if it failed. The caller now owns the volume memory and is responsible for deleting it. After calling once this will return NULL until the next load is called. Resets the state to READY. */
    VolumeData* getVolume();
    
    /** Current state of the loader */
    State getState();
    
    /** More details about what's going on */
    std::string getStateMessage() const;
    
private:
    VolumeData* volume;
    ID id;
    State state;
    std::string stateMessage;
    
    /** Actual loading work */
    void load();
	void loadTIFF(std::string&);

    
    /** The modality LUT transforms device-dependent values to device-independent modality values. For example, it will transform raw UINT16 CT data values into signed CT Hounsfield units. It uses the slope and intercept stored in the DICOM dataset to transform values. */
    template <typename T>
    void applyModalityLUT(double slope, double intercept)
    {
        stateMessage = "Applying Modality Transformation";

        T* buffer = (T*)volume->data;
        volume->minVoxelValue = std::numeric_limits<int>::infinity();
        volume->maxVoxelValue = -std::numeric_limits<int>::infinity();
        for (int i = 0; i < volume->getNumVoxels(); i++) {
            *buffer = static_cast<T>((*buffer) * slope + intercept);
            if (*buffer > volume->maxVoxelValue) volume->maxVoxelValue = *buffer;
            if (*buffer < volume->minVoxelValue) volume->minVoxelValue = *buffer;
            buffer++;
        }

		float nl = gl::normalize<T>(volume->minVoxelValue);
		float nr = gl::normalize<T>(volume->maxVoxelValue);
		volume->visible_.width(nl, nr);
    }
    
    /** This is used instead of modality LUT if the modality is unknown */
    template <typename T>
    void calculateMinMax()
    {
        stateMessage = "Calculating Min/Max Values";

        T* buffer = (T*)volume->data;
        volume->minVoxelValue = std::numeric_limits<int>::infinity();
        volume->maxVoxelValue = -std::numeric_limits<int>::infinity();
        for (int i = 0; i < volume->getNumVoxels(); i++) {
            if (*buffer > volume->maxVoxelValue) volume->maxVoxelValue = *buffer;
            if (*buffer < volume->minVoxelValue) volume->minVoxelValue = *buffer;
            buffer++;
        }

		float nl = gl::normalize<T>(volume->minVoxelValue);
		float nr = gl::normalize<T>(volume->maxVoxelValue);
		volume->visible_.width(nl, nr);
    }
};

#endif // __MEDLEAP_VOLUME_LOADER__