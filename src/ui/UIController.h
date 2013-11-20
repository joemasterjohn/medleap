#ifndef __MEDLEAP_UI_CONTROLLER__
#define __MEDLEAP_UI_CONTROLLER__

#include "render/Renderer3D.h"
#include "ui/CameraControl3D.h"
#include "KeyboardMouseListener.h"

class UIController : public KeyboardMouseListener
{
public:
    UIController(Renderer3D* renderer);
    
    void update();
    
    void keyboardInput(GLFWwindow* window, int key, int action, int mods);
    void mouseButton(GLFWwindow* window, int button, int action, int mods);
    void mouseMotion(GLFWwindow* window, double x, double y);
    void scroll(GLFWwindow* window, double dx, double dy);
    
private:
    CameraControl3D cameraControl;
};

#endif // __MEDLEAP_UI_CONTROLLER__