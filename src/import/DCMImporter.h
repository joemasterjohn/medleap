#ifndef __MEDLEAP_VOLUME_IMPORTER__
#define __MEDLEAP_VOLUME_IMPORTER__

#include <vector>
#include <string>
#include "volume/VolumeData.h"

/**
 This importer will work with directories containing DICOM files. A directory can store several
 DICOM series of different modalities. For example, there might be 3 unique CT series and a
 single MR series all stored in the same directory. This importer provides a method to scan
 the directory and determine all of these unique series IDs. It also provides a method to use a
 series ID to import the relevant images into a VolumeData object.
 */
class DCMImporter
{
public:
    
    /** Stores the ID and modality of a DICOM series. The modality will be either "CT" or "MR". The directory is the containing directory of the series. */
    struct SeriesID
    {
        std::string id;
        std::string modality;
        std::string directory;
    };
    
    DCMImporter();
    
    ~DCMImporter();
    
    /** This scans a directory for unique MR and CT series IDs. */
    std::vector<SeriesID> findSeriesIDs(const char* directory);
    
    /** This will import the first series found in the directory, regardless of how many series are stored inside. */
    VolumeData* importVolume(const char* directory);
    
    /** Load a specific series. */
    VolumeData* importVolume(SeriesID series);
};

#endif // __MEDLEAP_VOLUME_IMPORTER__
