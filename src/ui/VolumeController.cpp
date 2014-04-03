#include "VolumeController.h"
#include "math/Transform.h"

using namespace glmath;

VolumeController::VolumeController()
{
    mouseDragLeftButton = false;
    mouseDragRightButton = false;
}

VolumeController::~VolumeController()
{
}

VolumeRenderer* VolumeController::getRenderer()
{
    return &renderer;
}

void VolumeController::setVolume(VolumeData* volume)
{
    this->volume = volume;
    renderer.setVolume(volume);
}

bool VolumeController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
    return true;
}

bool VolumeController::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mouseDragLeftButton = action == GLFW_PRESS;
        if (mouseDragLeftButton) {
            dragStartView = renderer.getCamera().getView();
        }
        renderer.setMoving(mouseDragLeftButton);
    }
    
    return true;
}

bool VolumeController::mouseMotion(GLFWwindow* window, double x, double y)
{
    if (!renderer.getViewport().contains(x, y))
        return true;

    
    if (mouseDragLeftButton) {
        double dx = x - dragStartX;
        double dy = y - dragStartY;
        double pitch = dy * 0.01;
        double yaw = dx * 0.01;
        
        Mat4 m1 = rotation(pitch, dragStartView.row(0));
        Mat4 m2 = rotation(yaw, dragStartView.row(1));
        renderer.getCamera().setView(dragStartView * m1 * m2);
        renderer.markDirty();
    } else {
        dragStartX = x;
        dragStartY = y;
    }

    return true;
}

bool VolumeController::scroll(GLFWwindow* window, double dx, double dy)
{
    if (!mouseDragLeftButton) {
        renderer.getCamera().translateBackward(dy * 0.2);
        renderer.markDirty();
    }
    return true;
}