#ifndef medleap_ColorPickController_h
#define medleap_ColorPickController_h

#include "layers/Controller.h"
#include "ColorPickRenderer.h"
#include "util/Color.h"
#include "leap/HandTriggerTracker.h"
#include <functional>

class ColorPickController : public Controller
{
public:
	ColorPickController();

	ColorPickRenderer* getRenderer() override { return &renderer; }

	/** Adds a listener that will be notified when a color is picked. The callback will be removed after receiving notification. */
	void addCallback(std::function<void(const Color&)> callback);

	bool mouseMotion(GLFWwindow* window, double x, double y) override;
	bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;

	const ColorHSV& color() const { return mColor; }
	void color(const Color& color) { mColor = color.hsv(); renderer.choose(mColor); }

private:
	std::vector<std::function<void(const Color&)>> callbacks_;
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
