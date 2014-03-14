#ifndef __medleap__HistogramController__
#define __medleap__HistogramController__

#include "ui/Controller.h"
#include "render/HistogramRenderer.h"

class HistogramController
{
public:
    HistogramController();
    ~HistogramController();
    
    HistogramRenderer* getRenderer();
    
private:
    HistogramRenderer renderer;
};

#endif /* defined(__medleap__HistogramController__) */
