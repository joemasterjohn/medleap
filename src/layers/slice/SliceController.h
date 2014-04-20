#ifndef __medleap__SliceController__
#define __medleap__SliceController__

#include "layers/Controller.h"
#include "SliceRenderer.h"

/** Controls slice rendering layer */
class SliceController : public Controller
{
public:
    SliceController();
    ~SliceController();
    SliceRenderer* getRenderer();
    void setVolume(VolumeData* volume);
    
    bool keyboardInput(GLFWwindow* window, int key, int action, int mods) override;
    bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
    bool mouseMotion(GLFWwindow* window, double x, double y);
    bool scroll(GLFWwindow* window, double dx, double dy);
    
private:
    SliceRenderer renderer;
    VolumeData* volume;
    
    bool mouseLeftDrag;
    double mouseAnchorX;
    double mouseAnchorY;
    int anchorSliceIndex;
};

#endif /* defined(__medleap__SliceController__) */
