#ifndef medleap_ColorPickController_h
#define medleap_ColorPickController_h

#include "layers/Controller.h"
#include "ColorPickRenderer.h"
#include "Color.h"

class ColorPickController : public Controller
{
public:
	ColorPickController();

	ColorPickRenderer* getRenderer() override { return &renderer; }

	bool mouseMotion(GLFWwindow* window, double x, double y) override;
	bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;

	const ColorHSV& color() const { return mColor; }

private:
	ColorHSV mColor;
	ColorPickRenderer renderer;
	bool mChooseColor;
	bool mChooseAlpha;
	bool mChooseValue;
};

#endif // medleap_ColorPickController_h
