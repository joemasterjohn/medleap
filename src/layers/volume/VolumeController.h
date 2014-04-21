#ifndef __medleap__VolumeController__
#define __medleap__VolumeController__

#include "layers/Controller.h"
#include "layers/volume/VolumeRenderer.h"

/** Main controller for 3D mode */
class VolumeController : public Controller
{
public:
    VolumeController();
    ~VolumeController();
    VolumeRenderer* getRenderer();
    void setVolume(VolumeData* volume);
    
    bool keyboardInput(GLFWwindow* window, int key, int action, int mods) override;
    bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
    bool mouseMotion(GLFWwindow* window, double x, double y) override;
    bool scroll(GLFWwindow* window, double dx, double dy) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;

private:
    VolumeRenderer renderer;
    VolumeData* volume;
    
    // camera control
    bool mouseDragLeftButton;
    bool mouseDragRightButton;
    double dragStartX;
    double dragStartY;
	gl::Mat4 dragStartView;
};

#endif /* defined(__medleap__VolumeController__) */
