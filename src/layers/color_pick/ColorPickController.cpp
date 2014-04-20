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
		float r =min(1.0f, p.length() / circleRect.width * 2);

		mColor.hue(p.anglePositive());
		mColor.saturation(r);
		renderer.choose(mColor);
	}
	else if (mChooseAlpha) {
		Rectangle<float> r = renderer.alphaRect();
		float alpha = max(0.0f, min(1.0f, (static_cast<float>(y) - r.bottom()) / r.height));
		mColor.alpha(alpha);
		renderer.choose(mColor);
	}
	else if (mChooseValue) {
		Rectangle<float> r = renderer.valueRect();
		float value = max(0.0f, min(1.0f, (static_cast<float>(y) - r.bottom()) / r.height));
		mColor.value(value);
		renderer.choose(mColor);
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
			mouseMotion(window, x, y);
		}

		Rectangle<float> valueRect = renderer.valueRect();
		if (valueRect.contains(x, y)) {
			mChooseValue = true;
			mouseMotion(window, x, y);
		}
	}

	return true;
}