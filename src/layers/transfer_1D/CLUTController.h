#ifndef __medleap__CLUTController__
#define __medleap__CLUTController__

#include "layers/Controller.h"
#include "CLUTRenderer.h"
#include "CLUT.h"
#include "layers/volume/VolumeRenderer.h"
#include "layers/slice/SliceRenderer.h"
#include <vector>

/** User input controller for color look-up tables */
class CLUTController : public Controller
{
public:
    CLUTController();
    ~CLUTController();
    CLUTRenderer* getRenderer();
    void setVolumeRenderer(VolumeRenderer* volumeRenderer);
    void setSliceRenderer(SliceRenderer* sliceRenderer);
    
    CLUT& getActiveCLUT();
    

    bool keyboardInput(GLFWwindow* window, int key, int action, int mods);
    
    bool mouseButton(GLFWwindow* window, int button, int action, int mods);
    
    bool mouseMotion(GLFWwindow* window, double x, double y);
    
private:
    CLUTRenderer renderer;
    VolumeRenderer* volumeRenderer;
    SliceRenderer* sliceRenderer;
    std::vector<CLUT> cluts;
    int activeCLUT;
};
#endif /* defined(__medleap__CLUTController__) */
