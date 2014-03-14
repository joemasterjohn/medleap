#include "VolumeController.h"

VolumeController::VolumeController()
{
    cameraControl = new CameraControl3D(&renderer);
}

VolumeController::~VolumeController()
{
    delete cameraControl;
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
    cameraControl->keyboardInput(window, key, action, mods);
    return true;
}

bool VolumeController::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    cameraControl->mouseButton(window, button, action, mods);
    return true;
}

bool VolumeController::mouseMotion(GLFWwindow* window, double x, double y)
{
    cameraControl->mouseMotion(window, x, y);
    return true;
}

bool VolumeController::scroll(GLFWwindow* window, double dx, double dy)
{
    cameraControl->scroll(window, dx, dy);
    return true;
}