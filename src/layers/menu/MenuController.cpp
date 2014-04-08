#include "MenuController.h"
#include <iostream>
#include "util/Util.h"
#include "math/Vector2.h"

MenuController::MenuController()
{
	Menu& mainMenu = menuManager.createMenu("Main Menu");
	Menu& renderMenu = menuManager.createMenu("Render");
	renderMenu.createItem("Lighting");
	renderMenu.createItem("sRGB");
	renderMenu.createItem("Sampling Rate");
	renderMenu.createItem("Multisampling");
	renderMenu.createItem("RETURN", [this]{this->getMenuManager().popMenu(); });

	mainMenu.createItem("Exit", []{});
	mainMenu.createItem("Load", []{});
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

std::set<Leap::Gesture::Type> MenuController::requiredGestures()
{
	std::set<Leap::Gesture::Type> gestures;
	gestures.insert(Leap::Gesture::TYPE_SCREEN_TAP);
	gestures.insert(Leap::Gesture::TYPE_KEY_TAP);
	return gestures;
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
	double radians = Vec2d(x, y).anglePositive();
	return calcHighlightedMenu(radians);

}

int MenuController::calcHighlightedMenu(double radians)
{
	size_t numItems = menuManager.topMenu().getItems().size();
	double angleStep = gl::PI2/ numItems;
	return static_cast<int>(radians / angleStep + 0.5) % numItems;
}

bool MenuController::leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
{
	// Idea: maybe the tap gestures aren't teh greatest since they're insensitive. I can adjust the sensitivity or
	// instead use hold to press. Have a timer that checks how long finger hasn't moved (or velocity over last several frames). If
	// small, start pressing. If pressing for a second or so trigger.

	Leap::FingerList fingers = currentFrame.fingers();
	if (!fingers.isEmpty()) {
		Leap::Finger pointerFinger = fingers.frontmost();
		
		// if the menu opened by gesture, the central point will have been set
		// otherwise, we need to assume something directly over the leap
		Vec2 ctr(0, 250);
		Leap::Vector tip = pointerFinger.tipPosition();
		Vec2 v = Vec2(tip.x, tip.y) - ctr;
		float radians = v.anglePositive();
		int highlightedItem = calcHighlightedMenu(radians);
		renderer->highlight(highlightedItem);

		// check if a tap gesture was made to trigger menu
		Leap::GestureList gestures = currentFrame.gestures();
		if (!gestures.isEmpty()) {
			for (const Leap::Gesture& g : gestures) {
				if (g.type() == Leap::Gesture::TYPE_KEY_TAP || g.type() == Leap::Gesture::TYPE_SCREEN_TAP) {
					MenuItem& selected = menuManager.topMenu().getItems()[highlightedItem];
					selected.trigger();
				}
			}
		}

	}

	return false;
}