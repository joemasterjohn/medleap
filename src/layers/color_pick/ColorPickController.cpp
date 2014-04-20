#include "ColorPickController.h"
#include <algorithm>

using namespace std;
using namespace gl;

ColorPickController::ColorPickController() :
		mColor(0.0f, 1.0f, 1.0f, 1.0f),
		mChooseColor(false),
		mChooseAlpha(false),
		mChooseValue(false)
{
	renderer.choose(mColor);
}

bool ColorPickController::mouseMotion(GLFWwindow* window, double x, double y)
{
	if (mChooseColor) {
		Rectangle<float> circleRect = renderer.circleRect();
		Vec2 p = Vec2(x, y) - circleRect.center();
		float r = min(1.0f, p.length() / circleRect.width * 2);

		mColor.hue(p.anglePositive());
		mColor.saturation(r);
		renderer.choose(mColor);
	}
	else if (mChooseAlpha) {

	}
	else if (mChooseValue) {

	}

	return false;
}

bool ColorPickController::mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y)
{
	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
		mChooseColor = false;
		mChooseAlpha = false;
		mChooseValue = false;
	} else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) { 
		Rectangle<float> circleRect = renderer.circleRect();
		if ((Vec2(x, y) - circleRect.center()).length() < circleRect.width/2) {
			mChooseColor = true;
			mouseMotion(window, x, y);
		}

		Rectangle<float> alphaRect = renderer.alphaRect();
		if (alphaRect.contains(x, y)) {
			mChooseAlpha = true;
		}
	}

	return true;
}