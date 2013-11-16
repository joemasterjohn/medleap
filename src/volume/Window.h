#ifndef __MEDLEAP_VOLUME_WINDOW__
#define __MEDLEAP_VOLUME_WINDOW__

#include "GL/glew.h"

/** 
 * Used to describe the brightness and contrast of an image.
 * Pixels map from [min, max] to [black, white]. Values less than
 * min are clamped to black; values greater than max clamp to white.
 */
class Window
{
public:
    Window(float center, float width);
    
    float getCenter();
    
    /** Calculates the center in [0, 1] range with the given pixel type */
    float getCenterNormalized(GLenum type);
    float getWidth();
    
    /** Calculates the width in [0, 1] range with the given pixel type */
    float getWidthNormalized(GLenum type);
    float getMin();
    float getMax();
    
    void setCenter(float center);
    void setWidth(float width);
    
private:
    float center, width, min, max;
    
    void updateMinMax();
};

#endif // __MEDLEAP_VOLUME_WINDOW__