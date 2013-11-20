#ifndef __medleap__Renderer3D__
#define __medleap__Renderer3D__

#include "math/Matrix4.h"

class Renderer3D
{
public:
    
    void init();
    void resize(int width, int height);
    void draw();
    
private:
    cgl::Mat4 model;
};

#endif /* defined(__medleap__Renderer3D__) */
