#ifndef __medleap__Controller2D__
#define __medleap__Controller2D__

#include "ui/Controller.h"
#include "render/SliceRenderer2D.h"

/** Main controller for 2D mode */
class Controller2D : public Controller
{
public:
    Controller2D();
    ~Controller2D();
    SliceRenderer2D* getRenderLayer();
    void setVolume(VolumeData* volume);
    
    bool keyboardInput(GLFWwindow* window, int key, int action, int mods);
    bool mouseButton(GLFWwindow* window, int button, int action, int mods);
    bool mouseMotion(GLFWwindow* window, double x, double y);
    bool scroll(GLFWwindow* window, double dx, double dy);
    
private:
    SliceRenderer2D renderer;
    VolumeData* volume;
};

#endif /* defined(__medleap__Controller2D__) */
