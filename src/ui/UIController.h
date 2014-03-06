#ifndef __MEDLEAP_UI_CONTROLLER__
#define __MEDLEAP_UI_CONTROLLER__

#include "render/SliceRenderer2D.h"
#include "render/Renderer3D.h"
#include "ui/CameraControl3D.h"
#include "KeyboardMouseListener.h"

class UIController : public KeyboardMouseListener
{
public:
    UIController(SliceRenderer2D* renderer2D, Renderer3D* renderer);
    
    void update();
    
    void keyboardInput(GLFWwindow* window, int key, int action, int mods);
    void mouseButton(GLFWwindow* window, int button, int action, int mods);
    void mouseMotion(GLFWwindow* window, double x, double y);
    void scroll(GLFWwindow* window, double dx, double dy);
    void setVolume(VolumeData* volume);
    
    bool mode3D;

private:
    VolumeData* volume;
    SliceRenderer2D* renderer2D;
    CameraControl3D cameraControl;
};

#endif // __MEDLEAP_UI_CONTROLLER__