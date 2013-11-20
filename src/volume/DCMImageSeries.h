#ifndef __MEDLEAP_DCM_IMAGE_SERIES__
#define __MEDLEAP_DCM_IMAGE_SERIES__

#include "VolumeData.h"
#include <string>
#include "math/Matrix3.h"
#include "gdcmReader.h"
#include "gdcmAttribute.h"

/** DICOM image series */
class DCMImageSeries : public VolumeData
{
public:
    
    /** The type of image data */
    enum Modality
    {
        CT,
        MR,
        UNKNOWN
    };
    
    /** Identifies a DICOM image series on disk */
    struct ID
    {
        std::string uid;
        std::string directory;
        Modality modality;
        unsigned int numImages;
    };
    
    /** Returns the modality of the data */
    Modality getModality();
    
    /** Windows that store values of interest */
    std::vector<Window>& getWindows();
    
    /** Matrix that transforms DICOM image space (+X right, +Y down) to patient space (+X = left, +Y = posterior, +Z = superior) */
    const cgl::Mat3& getPatientBasis() const;
    
    /** Checks if the DICOM tag <G,E> exists in the data set */
    bool hasValue(uint16_t G, uint16_t E);
    
    /** Returns a value of type T with the DICOM tag <G,E>. */
    template <typename T, uint16_t G, uint16_t E> T getValue() {
        gdcm::Attribute<G,E> at;
        at.SetFromDataSet(reader.GetFile().GetDataSet());
        return at.GetValue();
    }
    
    /** Returns a value of type T with the DICOM tag <G,E>. */
    template <typename T, uint16_t G, uint16_t E> T getValues() {
        gdcm::Attribute<G,E> at;
        at.SetFromDataSet(reader.GetFile().GetDataSet());
        return at.GetValues();
    }
    
    /** This will search a directory to find all unique CT or MT image series. */
    static std::vector<ID> search(const char* directoryPath);
    
    /** Loads the specified image series */
    static DCMImageSeries* load(ID seriesID);
    
    /** This will load the first image series found in a directory */
    static DCMImageSeries* load(const char* directoryPath);
    
    /** Returns all of the file names associated with an image series, sorted by Z */
    static std::vector<std::string> sortFiles(ID seriesID);
    
private:
    gdcm::Reader reader;
    Modality modality;
    cgl::Mat3 orientation;
    std::vector<Window> windows;

    /** Can only construct by using static methods */
    DCMImageSeries(int width, int height, int depth, GLenum format, GLenum type);
};

#endif // __MEDLEAP_DCM_IMAGE_SERIES__