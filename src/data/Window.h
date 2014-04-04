#ifndef __MEDLEAP_VOLUME_WINDOW__
#define __MEDLEAP_VOLUME_WINDOW__

#include "gl/glew.h"

/** 
 * Describes a range of pixels that will be visible. For example, one data set
 * may contain values in the range [0, 65535] (16 bits of information per voxel).
 * If only the values in [0, 300] are interesting for visualization a "window"
 * can be used to map those values into the displayable range. This is common
 * with MIP visualization to get rid of uninteresting information (air, for example)
 * in the volume.
 *
 * This class stores a values that describe a range of interest as [min, max]. Data values
 * <= min will map to black; data values >= max will map to white; data values between
 * min and max will be linearly interpolated. DICOM files store these windows with
 * center/width values instead of min/max: the center is (max+min)/2 and the width
 * is max-min. These values describe the "value of interest" LUT that transforms modality
 * pixel values into meaningful values for the visualization.
 */
class Window
{
public:
    /** Creates a default window with default range of [TYPE_MIN, TYPE_MAX] */
    Window(GLenum type);
    
    /** Sets the window using real values */
    void setReal(float center, float width);
    
    /** Sets the window using normalized values and a type */
    void setNorm(float center, float width);

    /** Returns the real center */
    float getCenterReal();

    /** Returns the real width */
    float getWidthReal();
    
    /** Returns the real minimum value */
    float getMinReal();
    
    /** Returns the real maximum value */
    float getMaxReal();
    
    /** Returns the center in [0, 1] with the stored pixel type */
    float getCenterNorm();
    
    /** Returns the width in [0, 1] with the stored pixel type */
    float getWidthNorm();
    
    /** Returns the minimum value in [0, 1] with the stored pixel type */
    float getMinNorm();
    
    /** Returns the maximum value in [0, 1] with the stored pixel type */
    float getMaxNorm();
    
private:
    float centerReal, centerNorm;
    float widthReal, widthNorm;
    float minReal, minNorm;
    float maxReal, maxNorm;
    float typeMin, typeRange;
};

#endif // __MEDLEAP_VOLUME_WINDOW__