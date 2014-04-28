#include "MenuController.h"
#include <iostream>
#include "util/Util.h"
#include "gl/math/Math.h"
#include "main/MainController.h"

#include "DirectoryMenu.h"
#include "MainMenu.h"

using namespace std;
using namespace gl;


MenuController::MenuController() :
	transition_(std::chrono::milliseconds(250))
{
}

MenuController::~MenuController()
{
}

MenuRenderer* MenuController::getRenderer()
{
	return &renderer;
}

void MenuController::hideMenu()
{
	finger_tracker_.tracking(false);
	renderer.highlight(-1);
	renderer.progress_ = 0.0f;
	transition_.state(Transition::State::decrease);
}

void MenuController::showMainMenu()
{
	Menu* m = new MainMenu;
	menu_ = std::unique_ptr<Menu>(m);
	renderer.menu(m);
	transition_.state(Transition::State::increase);
}

void MenuController::showContextMenu()
{
	Controller* focusLayer = MainController::getInstance().focusLayer();
	if (focusLayer) {
		menu_ = move(focusLayer->contextMenu());
		renderer.menu(menu_.get());
		transition_.state(Transition::State::increase);
	}
}

bool MenuController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		if (transition_.full()) {
			hideMenu();
		} else if (transition_.empty()) {
			showMainMenu();
		}
	}
	else if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		if (transition_.full()) {
			hideMenu();
		} else if (transition_.empty()) {
			showContextMenu();
		}
	}

	return transition_.empty();
}

bool MenuController::mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y)
{
	if (transition_.empty())
		return true;

	if (transition_.full() && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		int selected = calcHighlightedMenu(x, y);
		(*menu_)[selected].trigger();
	}

	return false;
}

bool MenuController::mouseMotion(GLFWwindow* window, double x, double y)
{
	if (transition_.empty())
		return true;

	int item = calcHighlightedMenu(x, y);
	renderer.highlight(item);

	return false;
}

int MenuController::calcHighlightedMenu(double x, double y)
{
    x = (x - renderer.getViewport().width / 2.0);
    y = (y - renderer.getViewport().height / 2.0);
	double radians = Vec2d(x, y).anglePositive();
	return calcHighlightedMenu(radians);
}

int MenuController::calcHighlightedMenu(double radians)
{
	if (!menu_)
		return -1;

	size_t numItems = menu_->getItems().size();
	double angleStep = gl::two_pi / numItems;
	return static_cast<int>(radians / angleStep) % numItems;
}

bool MenuController::leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
{
	if (transition_.empty()) {
		Leap::GestureList gestures = currentFrame.gestures();
		for (const Leap::Gesture& g : gestures) {
			if (g.type() == Leap::Gesture::TYPE_CIRCLE) {
				Leap::CircleGesture circle(g);
				if (circle.progress() > 1) {
					if (currentFrame.fingers().count() > 1)
						showMainMenu();
					else
						showContextMenu();
				}
			}
		}
	}
	else {

		finger_tracker_.update(leapController);

		renderer.highlight(-1);
		renderer.progress_ = 0.0f;

		if (finger_tracker_.tracking()) {
			Leap::Vector delta = finger_tracker_.posDelta(leapController.frame());
			Vec2 v = { delta.x, delta.y };
			float radians = v.anglePositive();
			float radius = v.length();

			std::cout << radius << std::endl;

			renderer.leap.x = v.x;
			renderer.leap.y = v.y;

			if (radius > 5.0f) {
				int highlightedItem = calcHighlightedMenu(radians);
				selected_ = highlightedItem;
				renderer.highlight(highlightedItem);

				if (radius > 35.0f) {
					renderer.progress_ = gl::clamp((radius - 35.0f) / 100.0f, 0.0f, 1.0f);
					if (renderer.progress_ >= 1.0f) {
						(*menu_)[highlightedItem].trigger();
						finger_tracker_.tracking(false);
					}

				}
			}
		}
	}

	return false;
}

void MenuController::update(std::chrono::milliseconds elapsed)
{
	transition_.update(elapsed);
	renderer.visibility_ = transition_.progress();
	std::cout << transition_.progress() << std::endl;
}