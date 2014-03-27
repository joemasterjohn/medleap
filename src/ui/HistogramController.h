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
    ~HistogramController();
    HistogramRenderer* getRenderer();
    void setVolume(VolumeData* volume);
    
    bool mouseMotion(GLFWwindow* window, double x, double y);
    bool mouseButton(GLFWwindow* window, int button, int action, int mods);
    
private:
    bool lMouseDrag, rMouseDrag;
    HistogramRenderer renderer;
    VolumeData* volume;
    Histogram* histogram;
    GLubyte* transfer1DPixels;
    int transferWidth1D, transferHeight1D;
    
    void updateTransferTex1D();
};

#endif /* defined(__medleap__HistogramController__) */
