#include "CameraControl3D.h"
#include "math/Transform.h"

using namespace cgl;

CameraControl3D::CameraControl3D(VolumeRenderer* renderer) : renderer(renderer), mouseDragLeftButton(false)
{
}

void CameraControl3D::keyboardInput(GLFWwindow *window, int key, int action, int mods)
{
}

void CameraControl3D::mouseButton(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mouseDragLeftButton = action == GLFW_PRESS;
        if (mouseDragLeftButton) {
            dragStartView = renderer->getCamera().getView();
        }
        renderer->setMoving(mouseDragLeftButton);
    }
}

void CameraControl3D::mouseMotion(GLFWwindow *window, double x, double y)
{
    if (mouseDragLeftButton) {
        double dx = x - dragStartX;
        double dy = y - dragStartY;
        double pitch = dy * 0.01;
        double yaw = dx * 0.01;
        
        Mat4 m1 = rotation(pitch, dragStartView.row(0));
        Mat4 m2 = rotation(yaw, dragStartView.row(1));
        renderer->getCamera().setView(dragStartView * m1 * m2);
        renderer->markDirty();
    } else {
        dragStartX = x;
        dragStartY = y;
    }
}

void CameraControl3D::scroll(GLFWwindow *window, double dx, double dy)
{
    if (!mouseDragLeftButton) {
        renderer->getCamera().translateBackward(dy * 0.2);
        renderer->markDirty();
    }
}