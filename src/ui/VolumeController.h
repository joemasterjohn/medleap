#ifndef __medleap__VolumeController__
#define __medleap__VolumeController__

#include "ui/Controller.h"
#include "render/Renderer3D.h"
#include "ui/CameraControl3D.h"

/** Main controller for 3D mode */
class VolumeController : public Controller
{
public:
    VolumeController();
    ~VolumeController();
    Renderer3D* getRenderLayer();
    void setVolume(VolumeData* volume);
    
    bool keyboardInput(GLFWwindow* window, int key, int action, int mods);
    bool mouseButton(GLFWwindow* window, int button, int action, int mods);
    bool mouseMotion(GLFWwindow* window, double x, double y);
    bool scroll(GLFWwindow* window, double dx, double dy);
    
private:
    Renderer3D renderer;
    CameraControl3D* cameraControl;
    VolumeData* volume;
};

#endif /* defined(__medleap__VolumeController__) */
