#include "UIController.h"

UIController::UIController(Renderer3D* renderer) : cameraControl(renderer)
{
}

void UIController::keyboardInput(GLFWwindow *window, int key, int action, int mods)
{
    cameraControl.keyboardInput(window, key, action, mods);
}

void UIController::mouseButton(GLFWwindow *window, int button, int action, int mods)
{
    cameraControl.mouseButton(window, button, action, mods);
}

void UIController::mouseMotion(GLFWwindow *window, double x, double y)
{
    cameraControl.mouseMotion(window, x, y);
}

void UIController::scroll(GLFWwindow *window, double dx, double dy)
{
    cameraControl.scroll(window, dx, dy);
}