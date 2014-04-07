#include "MenuController.h"
#include <iostream>
#include "util\Util.h"

MenuController::MenuController()
{
	Menu& mainMenu = menuManager.createMenu("Main Menu");
	Menu& renderMenu = menuManager.createMenu("Render");
	renderMenu.createItem("Lighting");
	renderMenu.createItem("sRGB");
	renderMenu.createItem("Sampling Rate");
	renderMenu.createItem("Multisampling");
	renderMenu.createItem("RETURN", [this]{this->getMenuManager().popMenu(); });

	mainMenu.createItem("Foo");
	mainMenu.createItem("Cat", []{ std::cout << "pressed cat" << std::endl; });


	mainMenu.createItem("Render",
		[this] {
		this->getMenuManager().pushMenu("Render");
		}
	);

	menuManager.pushMenu("Main Menu");

	// load, exit

	renderer = new MenuRenderer(&menuManager);
}

MenuController::~MenuController()
{
}

MenuRenderer* MenuController::getRenderer()
{
	return renderer;
}

MenuManager& MenuController::getMenuManager()
{
	return menuManager;
}

bool  MenuController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
	return false;
}

bool  MenuController::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		y = renderer->getViewport().height - y - 1;
		int item = calcHighlightedMenu(x, y);
		menuManager.topMenu().getItems()[item].trigger();
	}
	return false;
}

bool  MenuController::mouseMotion(GLFWwindow* window, double x, double y)
{
	int item = calcHighlightedMenu(x, y);
	renderer->highlight(item);

	return false;
}

bool  MenuController::scroll(GLFWwindow* window, double dx, double dy)
{
	return false;
}

int MenuController::calcHighlightedMenu(double x, double y)
{
	x = 2.0 * x / renderer->getViewport().width - 1.0;
	y = 2.0 * y / renderer->getViewport().height - 1.0;

	unsigned numItems = menuManager.topMenu().getItems().size();

	double radians = std::atan2(y, x);
	if (radians < 0)
		radians += 2.0 * PI;
	double angleStep = PI * 2.0 / numItems;

	return static_cast<int>(radians / angleStep + 0.5) % numItems;
}