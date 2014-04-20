#include "ColorPickController.h"
#include <algorithm>

using namespace std;
using namespace gl;
using namespace Leap;

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

	return false;
}

bool ColorPickController::leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
{
	static int framesSinceLastTrigger = 100;
	static bool tracking = false;

	FingerList fingers = currentFrame.fingers();
	if (fingers.count() > 1 && fingers.count() < 4) {

		// assuming RIGHT HAND
		Finger index = fingers.frontmost();
		Finger thumb = fingers.leftmost();
		double relV = abs(thumb.tipVelocity().magnitude() - index.tipVelocity().magnitude());

		static float prevValue;
		static Vec3 ref;

		if (thumb.tipVelocity().magnitude() > 200 && index.tipVelocity().magnitude() < 50 && framesSinceLastTrigger > 80) {
			tracking = !tracking;
			framesSinceLastTrigger = 0;
			if (tracking) {
				prevValue = mColor.value();
				ref = Vec3(index.tipPosition().x, index.tipPosition().y, index.tipPosition().z);
				tracking = true;
			}
			cout << "tracking : " << tracking << endl;
		}

		if (framesSinceLastTrigger < 100)
			framesSinceLastTrigger++;


		if (tracking) {
			Vec3 tip(index.tipPosition().x, index.tipPosition().y, index.tipPosition().z);
			tip -= ref;
			float hue = Vec2(tip.x, tip.y).anglePositive();
			float sat = Vec2(tip.x, tip.y).length() / 50.0f;
			float val = prevValue + (tip.z / 50.0f);
			val = max(min(val, 1.0f), 0.0f);
			sat = max(min(sat, 1.0f), 0.0f);
			mColor.hue(hue);
			mColor.saturation(sat);
			mColor.value(val);
			renderer.choose(mColor);
		}
	}
	else {
		if (tracking)
			cout << "tracking : 0" << endl;
		tracking = false;
		framesSinceLastTrigger = 100;
	}

	renderer.tracking(tracking);

	if (fingers.count() >= 4) {
		

		// height of hand
		Hand hand = currentFrame.hands().frontmost();
		float y = hand.palmPosition().y;
		
		float a = max(min(1.0f, (y - 250) / 100), -1.0f) * 0.5 + 0.5;
		mColor.alpha(a);
		renderer.choose(mColor);
	}

	return false;
}