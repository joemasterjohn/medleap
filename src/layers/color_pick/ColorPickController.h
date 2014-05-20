#ifndef medleap_ColorPickController_h
#define medleap_ColorPickController_h

#include "layers/Controller.h"
#include "util/Color.h"
#include "leap/LPose.h"
#include "gl/geom/Rectangle.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "gl/math/Math.h"
#include "gl/util/Draw.h"
#include "util/TextRenderer.h"
#include <functional>

class ColorPickController : public Controller
{
public:
	ColorPickController();
	void clearCallbacks();
	void addCallback(std::function<void(const Color&)> callback);
	void draw() override;
	bool keyboardInput(GLFWwindow* window, int key, int action, int mods) override { return false; }
	bool mouseMotion(GLFWwindow* window, double x, double y) override;
	bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;
	void gainFocus() override;
	void loseFocus() override;
	bool modal() const override { return true; }
	const ColorHSV& color() const { return color_; }
	void color(const Color& color) { color_ = color.hsv(); }

private:
	enum class State { idle, choose_color, choose_alpha, choose_value };

	std::vector<std::function<void(const Color&)>> callbacks_;
	ColorHSV color_;
	LPose l_pose_;
	State state_;
	gl::Buffer geom_vbo_;
	gl::Program color_circle_prog_;
	gl::Program color_select_prog_;
	gl::Program gradient_prog_;
	gl::Rectangle<float> circle_rect_;
	gl::Rectangle<float> alpha_rect_;
	gl::Rectangle<float> value_rect_;
	gl::Rectangle<float> select_rect_;
	gl::Draw color_cursor_;
	gl::Mat4 projection_;
	gl::Vec2 m_leap_cursor;
	TextRenderer text_;

	void updateState(float x, float y);
	void chooseState(float x, float y);

	void resize() override;
	void quad(gl::Program prog, const gl::Rectangle<float>& rect);
	void updateText();
};

#endif // medleap_ColorPickController_h
