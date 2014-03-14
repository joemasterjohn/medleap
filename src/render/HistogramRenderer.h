#ifndef __medleap__HistogramRenderer__
#define __medleap__HistogramRenderer__

#include "render/Renderer.h"

class HistogramRenderer : public Renderer
{
public:
    void init();
    void draw();
    void resize(int width, int height);
    
private:
};

#endif /* defined(__medleap__HistogramRenderer__) */
