#ifndef medleap_ColorPickController_h
#define medleap_ColorPickController_h

#include "layers/Controller.h"
#include "ColorPickRenderer.h"
#include "Color.h"
#include "leap/HandTriggerTracker.h"

class ColorPickController : public Controller
{
public:
	ColorPickController();

	ColorPickRenderer* getRenderer() override { return &renderer; }

	bool mouseMotion(GLFWwindow* window, double x, double y) override;
	bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;

	const ColorHSV& color() const { return mColor; }

private:
	ColorHSV m_prev_color;
	ColorHSV mColor;
	ColorPickRenderer renderer;
	HandTriggerTracker handTrigger;
	bool mChooseColor;
	bool mChooseAlpha;
	bool mChooseValue;

	void leapChooseWidget(const Leap::Controller& controller);
	void leapUpdateColor(const Leap::Controller& controller);
	void leapUpdateAlpha(const Leap::Controller& controller);
	void leapUpdateValue(const Leap::Controller& controller);
};

#endif // medleap_ColorPickController_h
