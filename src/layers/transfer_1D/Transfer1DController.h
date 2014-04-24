#ifndef __medleap__Transfer1DController__
#define __medleap__Transfer1DController__

#include "layers/Controller.h"
#include "Transfer1DRenderer.h"
#include "data/VolumeData.h"
#include "layers/volume/VolumeRenderer.h"
#include "layers/slice/SliceRenderer.h"
#include "Histogram.h"

class Transfer1DController : public Controller
{
public:
    Transfer1DController();
    ~Transfer1DController();
    Transfer1DRenderer* getRenderer();
    void setVolume(VolumeData* volume);
    
    bool keyboardInput(GLFWwindow* window, int key, int action, int mods);
    bool mouseMotion(GLFWwindow* window, double x, double y);
    bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
    Histogram* histogram;

    void setVolumeRenderer(VolumeRenderer* volumeRenderer);
    void setSliceRenderer(SliceRenderer* sliceRenderer);
    
private:
    bool lMouseDrag, rMouseDrag;
    Transfer1DRenderer renderer;
    VolumeData* volume;
    GLubyte* transfer1DPixels;
    
    VolumeRenderer* volumeRenderer;
    SliceRenderer* sliceRenderer;
    std::vector<CLUT> cluts;
    int activeCLUT;
	CLUT::Marker* selected_;
    
    void updateTransferTex1D();
};

#endif /* defined(__medleap__Transfer1DController__) */
