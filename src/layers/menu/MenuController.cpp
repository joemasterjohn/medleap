#include "MenuController.h"

MenuController::MenuController()
{
}

MenuController::~MenuController()
{
}

MenuRenderer* MenuController::getRenderer()
{
	return &renderer;
}

bool  MenuController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
	return false;
}

bool  MenuController::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
	return false;
}

bool  MenuController::mouseMotion(GLFWwindow* window, double x, double y)
{
	return false;
}

bool  MenuController::scroll(GLFWwindow* window, double dx, double dy)
{
	return false;
}