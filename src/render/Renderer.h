#ifndef __medleap__Renderer__
#define __medleap__Renderer__

#include "gl/Viewport.h"

/** Rendering is done in layers so UI elements can be presented on top of the volume visualization in an arbitrary fashion. The rendering process will visit a stack of layers and render bottom to top. */
class Renderer
{
public:    
    /** Virtual destructor */
    virtual ~Renderer() {}
    
    /** Initialize resources. Called once upon loading. */
    virtual void init() = 0;
    
    /** Draw layer */
    virtual void draw() = 0;
    
    /** Rendering surface area is resized */
    virtual void resize(int width, int height) = 0;
    
    void setViewport(int x, int y, int width, int height)
    {
        bool changed = viewport.width != width || viewport.height != height;
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        
        if (changed)
            resize(width, height);
    }
    
    const cgl::Viewport& getViewport()
    {
        return viewport;
    }
    
protected:
    cgl::Viewport viewport;
};

#endif /* defined(__medleap__Renderer__) */
