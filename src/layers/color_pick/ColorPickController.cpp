#include "ColorPickController.h"
#include <algorithm>

using namespace std;
using namespace gl;
using namespace Leap;

ColorPickController::ColorPickController() :
		m_prev_color(0.0f, 1.0f, 1.0f, 1.0f),
		mColor(0.0f, 1.0f, 1.0f, 1.0f),
		mChooseColor(false),
		mChooseAlpha(false),
		mChooseValue(false)
{
	renderer.choose(mColor);

	handTrigger.engageFunction(std::bind(&ColorPickController::leapChooseWidget, this, std::placeholders::_1));
}

void ColorPickController::addCallback(std::function<void(const Color&)> callback)
{
	callbacks_.push_back(callback);
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
		else if (renderer.alphaRect().contains(x, y)) {
			mChooseAlpha = true;
			mouseMotion(window, x, y);
		}
		else if (renderer.valueRect().contains(x, y)) {
			mChooseValue = true;
			mouseMotion(window, x, y);
		}
		else if (renderer.previewRect().contains(x, y)) {
			for (std::function<void(const Color&)>& cb : callbacks_)
				cb(mColor);
			callbacks_.clear();
		}
	}

	return false;
}

bool ColorPickController::leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
{
	handTrigger.update(leapController);

	// DEBUG ONLY: REMOVE (change to highlight text indicating which funciton will be used)
	if (!handTrigger.tracking() && currentFrame.fingers().count() > 0) {
		Vector v = leapController.frame().fingers().frontmost().tipPosition();
		float x = v.x / 100.0f * renderer.getViewport().width / 2.0f + renderer.getViewport().center().x;
		float y = (v.y - 250) / 100.0f * renderer.getViewport().height / 2.0f + renderer.getViewport().center().y;
		renderer.leapCursor({ x,y });
	} else {
		renderer.leapCursor({ -200, -200 });
	}

	if (!handTrigger.tracking() && currentFrame.fingers().count() > 4) {
		for (std::function<void(const Color&)>& cb : callbacks_)
			cb(mColor);
		callbacks_.clear();
	}

	return false;
}

void ColorPickController::leapChooseWidget(const Leap::Controller& controller)
{
	Vector v = controller.frame().fingers().frontmost().tipPosition();
	float x = v.x / 100.0f * renderer.getViewport().width / 2.0f + renderer.getViewport().center().x;
	float y = (v.y - 250) / 100.0f * renderer.getViewport().height / 2.0f + renderer.getViewport().center().y;

	float vpw = renderer.getViewport().width;

	if (x <= vpw / 3.0f) {
		handTrigger.trackFunction(std::bind(&ColorPickController::leapUpdateAlpha, this, std::placeholders::_1));
	}
	else if (x <= vpw / 3.0f * 2.0f) {
		handTrigger.trackFunction(std::bind(&ColorPickController::leapUpdateColor, this, std::placeholders::_1));
	}
	else {
		handTrigger.trackFunction(std::bind(&ColorPickController::leapUpdateValue, this, std::placeholders::_1));
	}

	m_prev_color = mColor;
}

void ColorPickController::leapUpdateColor(const Leap::Controller& controller)
{
	Vec3 tip(handTrigger.deltaTipPos().x, handTrigger.deltaTipPos().y, handTrigger.deltaTipPos().z);

	float prevValue = 0.0f;
	float hue = Vec2(tip.x, tip.y).anglePositive();
	float sat = Vec2(tip.x, tip.y).length() / 50.0f;
	sat = max(min(sat, 1.0f), 0.0f);
	mColor.hue(hue);
	mColor.saturation(sat);
	renderer.choose(mColor);
}

void ColorPickController::leapUpdateAlpha(const Leap::Controller& controller)
{
	Vec3 tip(handTrigger.deltaTipPos().x, handTrigger.deltaTipPos().y, handTrigger.deltaTipPos().z);
	mColor.alpha(clamp(m_prev_color.alpha() + tip.y / 100.0f, 0.0f, 1.0f));
	renderer.choose(mColor);
}

void ColorPickController::leapUpdateValue(const Leap::Controller& controller)
{
	Vec3 tip(handTrigger.deltaTipPos().x, handTrigger.deltaTipPos().y, handTrigger.deltaTipPos().z);
	mColor.value(clamp(m_prev_color.value() + tip.y / 100.0f, 0.0f, 1.0f));
	renderer.choose(mColor);
}