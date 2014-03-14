#ifndef __MEDLEAP_CAMERA_CONTROL_3D__
#define __MEDLEAP_CAMERA_CONTROL_3D__

#include "render/VolumeRenderer.h"
#include "KeyboardMouseListener.h"

class CameraControl3D : public KeyboardMouseListener
{
public:
    CameraControl3D(VolumeRenderer* renderer);
    
    void update();
    
    void keyboardInput(GLFWwindow* window, int key, int action, int mods);
    void mouseButton(GLFWwindow* window, int button, int action, int mods);
    void mouseMotion(GLFWwindow* window, double x, double y);
    void scroll(GLFWwindow* window, double dx, double dy);
private:
    VolumeRenderer* renderer;
    bool mouseDragLeftButton;
    double dragStartX;
    double dragStartY;
    cgl::Mat4 dragStartView;
};

#endif // __MEDLEAP_CAMERA_CONTROL_3D__