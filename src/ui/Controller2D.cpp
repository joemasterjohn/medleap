#include "Controller2D.h"

Controller2D::Controller2D()
{
}

Controller2D::~Controller2D()
{
}

SliceRenderer2D* Controller2D::getRenderLayer()
{
    return &renderer;
}

void Controller2D::setVolume(VolumeData* volume)
{
    this->volume = volume;
    renderer.setVolume(volume);
}

bool Controller2D::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT) {
        renderer.setCurrentSlice(renderer.getCurrentSlice() + 1);
    }
    
    return true;
}

bool Controller2D::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    return true;
}

bool Controller2D::mouseMotion(GLFWwindow* window, double x, double y)
{
    return true;
}

bool Controller2D::scroll(GLFWwindow* window, double dx, double dy)
{
    return true;
}