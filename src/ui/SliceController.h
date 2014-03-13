#ifndef __medleap__SliceController__
#define __medleap__SliceController__

#include "ui/Controller.h"
#include "render/SliceRenderer.h"

/** Controls slice rendering layer */
class SliceController : public Controller
{
public:
    SliceController();
    ~SliceController();
    SliceRenderer* getRenderLayer();
    void setVolume(VolumeData* volume);
    
    bool keyboardInput(GLFWwindow* window, int key, int action, int mods);
    bool mouseButton(GLFWwindow* window, int button, int action, int mods);
    bool mouseMotion(GLFWwindow* window, double x, double y);
    bool scroll(GLFWwindow* window, double dx, double dy);
    
private:
    SliceRenderer renderer;
    VolumeData* volume;
};

#endif /* defined(__medleap__SliceController__) */
