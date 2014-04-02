#ifndef __medleap__CLUT__
#define __medleap__CLUT__

#include "math/Vector4.h"
#include <vector>

/** Color look-up table */
class CLUT
{
public:
    
    class ColorStop
    {
    public:
        ColorStop(float position, float r, float g, float b, float a);
        ColorStop(float position, cgl::Vec4 color);
        
        float getPosition();
        void setPosition(float position);
        
        cgl::Vec4 getColor();
        void setColor(float r, float g, float b, float a);
        void setColor(cgl::Vec4 color);
        
    private:
        float position;
        cgl::Vec4 color;
    };
    
    
    CLUT();
    ~CLUT();
    cgl::Vec4 getColor(float position);
    ColorStop& getColorStop(float position);
    void removeColorStop(float position);
    void addColorStop(float position, cgl::Vec4 color);
    void clearStops();
    
    void saveTexture();
    
private:
    std::vector<ColorStop> stops;
    
    /** Finds the index of the last color stop with position less or equal to pos */
    int findLeftStop(float pos);
    
    /** Finds the index of the first color stop with position greater or equal to pos */
    int findRightStop(float position);
    
    /** Finds the index of the color stop closest to pos */
    int findNearestStop(float pos);
};

#endif /* defined(__medleap__CLUT__) */
