#include "Controller3D.h"

Controller3D::Controller3D()
{
    cameraControl = new CameraControl3D(&renderer);
}

Controller3D::~Controller3D()
{
    delete cameraControl;
}

Renderer3D* Controller3D::getRenderLayer()
{
    return &renderer;
}

void Controller3D::setVolume(VolumeData* volume)
{
    this->volume = volume;
    renderer.setVolume(volume);
}

bool Controller3D::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
    cameraControl->keyboardInput(window, key, action, mods);
    return true;
}

bool Controller3D::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    cameraControl->mouseButton(window, button, action, mods);
    return true;
}

bool Controller3D::mouseMotion(GLFWwindow* window, double x, double y)
{
    cameraControl->mouseMotion(window, x, y);
    return true;
}

bool Controller3D::scroll(GLFWwindow* window, double dx, double dy)
{
    cameraControl->scroll(window, dx, dy);
    return true;
}