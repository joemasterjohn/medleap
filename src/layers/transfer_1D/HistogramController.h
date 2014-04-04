#ifndef __medleap__HistogramController__
#define __medleap__HistogramController__

#include "layers/Controller.h"
#include "HistogramRenderer.h"
#include "data/VolumeData.h"
#include "Histogram.h"

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
