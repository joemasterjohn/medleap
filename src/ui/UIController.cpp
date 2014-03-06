#include "UIController.h"

UIController::UIController(SliceRenderer2D* renderer2D ,Renderer3D* renderer) : mode3D(false), renderer2D(renderer2D), cameraControl(renderer)
{
}

void UIController::keyboardInput(GLFWwindow *window, int key, int action, int mods)
{
    if (mode3D)
        cameraControl.keyboardInput(window, key, action, mods);
    else {
        if (key == GLFW_KEY_RIGHT) {
            renderer2D->setCurrentSlice(renderer2D->getCurrentSlice() + 1);
        }
    }
    
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        volume->setNextWindow();
    }
}

void UIController::mouseButton(GLFWwindow *window, int button, int action, int mods)
{
    if (mode3D)
        cameraControl.mouseButton(window, button, action, mods);
}

void UIController::mouseMotion(GLFWwindow *window, double x, double y)
{
    if (mode3D)
        cameraControl.mouseMotion(window, x, y);
}

void UIController::scroll(GLFWwindow *window, double dx, double dy)
{
    if (mode3D)
        cameraControl.scroll(window, dx, dy);
}

void UIController::setVolume(VolumeData* volume)
{
    this->volume = volume;
}