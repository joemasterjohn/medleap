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
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        renderer.setOpacityScale(renderer.getOpacityScale() + 0.1f);
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        renderer.setOpacityScale(renderer.getOpacityScale() - 0.1f);
    }
    return true;
}

bool VolumeController::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mouseDragLeftButton = action == GLFW_PRESS;
        if (mouseDragLeftButton) {
            dragStartView = renderer.getCamera().getView();
        }
    }
    
//    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
//        mouseDragRightButton = action == GLFW_PRESS;
//        if (mouseDragRightButton) {
//            dragStartView = renderer.getCamera().getView();
//        }
//        renderer.setMoving(mouseDragRightButton);
//    }
    
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
        
        Mat4 m1 = rotation(static_cast<float>(pitch), dragStartView.row(0));
        Mat4 m2 = rotation(static_cast<float>(yaw), dragStartView.row(1));
        renderer.getCamera().setView(dragStartView * m1 * m2);
        renderer.markDirty();
    } else if (mouseDragRightButton) {
//        double dx = x - dragStartX;
//        double dy = y - dragStartY;
//        
//        Mat4 tm = translation(dx*0.002, dy*0.002, 0);
//        renderer.getCamera().setView(tm * dragStartView);
//        
    } else {
        dragStartX = x;
        dragStartY = y;
    }

    return true;
}

bool VolumeController::scroll(GLFWwindow* window, double dx, double dy)
{
    if (!mouseDragLeftButton) {
        renderer.getCamera().translateBackward(static_cast<float>(dy * 0.2f));
        renderer.markDirty();
    }
    return true;
}