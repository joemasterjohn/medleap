#include "MenuController.h"
#include <iostream>
#include "util/Util.h"
#include "math/Vector2.h"
#include "main/MainController.h"

#include "DirectoryMenu.h"
#include "MainMenu.h"

MenuController::MenuController()
{
	renderer = new MenuRenderer(&menus);
    menus.push(std::shared_ptr<Menu>(new MainMenu(&menus)));
	leapCenter.x = 0;
	leapCenter.y = 250;
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
	return menus;
}

void MenuController::setLeapCenter(const Vec2& center)
{
	this->leapCenter = center;
}


bool MenuController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
	return false;
}

bool MenuController::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (menus.visibility() < 1.0) return false;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		y = renderer->getViewport().height - y - 1;
		int selected = calcHighlightedMenu(x, y);
		menus.top()[selected].trigger();
	}
	return false;
}

bool MenuController::mouseMotion(GLFWwindow* window, double x, double y)
{
	if (menus.visibility() < 1.0) return false;

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
    x = (x - renderer->getViewport().width / 2.0);
    y = (y - renderer->getViewport().height / 2.0);
//	x = 2.0 * x / renderer->getViewport().width - 1.0;
//	y = 2.0 * y / renderer->getViewport().height - 1.0;
	double radians = Vec2d(x, y).anglePositive();
	return calcHighlightedMenu(radians);

}

int MenuController::calcHighlightedMenu(double radians)
{
	size_t numItems = menus.top().getItems().size();
	double angleStep = gl::PI2/ numItems;
	return static_cast<int>(radians / angleStep) % numItems;
}

bool MenuController::leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
{
	if (menus.visibility() < 1.0) return false;

	static long howLong = 0;
	static int lastHighlighted = -1;

	Leap::FingerList fingers = currentFrame.fingers();
	if (!fingers.isEmpty()) {

		Leap::Finger pointerFinger = fingers.frontmost();
		
		Leap::Vector tip = pointerFinger.tipPosition();
		Vec2 v = Vec2(tip.x, tip.y) - leapCenter;
		float radians = v.anglePositive();

		int highlightedItem = calcHighlightedMenu(radians);
		renderer->highlight(highlightedItem);

		if (pointerFinger.tipVelocity().magnitudeSquared() > 8000 || highlightedItem != lastHighlighted) {
			howLong = 0;
		}

		menus.setLeapProgress(howLong / 50.0f);
		if (howLong++ > 50) {
			menus.top()[highlightedItem].trigger();
			howLong = 0;
		}

		lastHighlighted = highlightedItem;
	}

	return false;
}

void MenuController::update(std::chrono::milliseconds elapsed)
{
	menus.update(elapsed);
}