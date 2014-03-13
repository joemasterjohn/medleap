#include "SliceController.h"

SliceController::SliceController()
{
}

SliceController::~SliceController()
{
}

SliceRenderer* SliceController::getRenderLayer()
{
    return &renderer;
}

void SliceController::setVolume(VolumeData* volume)
{
    this->volume = volume;
    renderer.setVolume(volume);
}

bool SliceController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        int sliceIndex = (renderer.getCurrentSlice() + 1) % volume->getDepth();
        renderer.setCurrentSlice(sliceIndex);
    } else if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        int sliceIndex = renderer.getCurrentSlice() - 1;
        if (sliceIndex < 0)
            sliceIndex = volume->getDepth() - 1;
        renderer.setCurrentSlice(sliceIndex);
    }
    
    return true;
}

bool SliceController::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    return true;
}

bool SliceController::mouseMotion(GLFWwindow* window, double x, double y)
{
    return true;
}

bool SliceController::scroll(GLFWwindow* window, double dx, double dy)
{
    return true;
}