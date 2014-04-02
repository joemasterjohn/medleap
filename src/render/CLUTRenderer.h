#ifndef __medleap__CLUTRenderer__
#define __medleap__CLUTRenderer__

#include "Renderer.h"

class CLUTRenderer : public Renderer
{
public:
    CLUTRenderer();
    ~CLUTRenderer();
    void init();
    void draw();
    void resize(int width, int height);
    
    
    // Have a shader render into a framebuffer to create the gradient texture
    // Vertex geometry will be created for each gradient point
    //
    // *---*---------------*
    // |   |               |
    // |   |               |
    // *---*---------------*
    
private:
};

#endif /* defined(__medleap__CLUTRenderer__) */
