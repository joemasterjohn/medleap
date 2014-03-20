#ifndef __medleap__HistogramController__
#define __medleap__HistogramController__

#include "ui/Controller.h"
#include "render/HistogramRenderer.h"
#include "volume/VolumeData.h"
#include "volume/Histogram.h"

class HistogramController : public Controller
{
public:
    HistogramController();
    HistogramRenderer* getRenderer();
    void setVolume(VolumeData* volume);
    
    bool mouseMotion(GLFWwindow* window, double x, double y);
    
private:
    HistogramRenderer renderer;
    VolumeData* volume;
    Histogram* histogram;
};

#endif /* defined(__medleap__HistogramController__) */
