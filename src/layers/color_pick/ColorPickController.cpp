#include "ColorPickController.h"
#include <algorithm>

using namespace std;
using namespace gl;
using namespace Leap;

ColorPickController::ColorPickController() :
		color_(0.0f, 1.0f, 1.0f, 1.0f),
		state_(State::idle)
{
	GLfloat vertices[] = {
		-1.0f, -1.0f,
		+1.0f, -1.0f,
		+1.0f, +1.0f,
		-1.0f, -1.0f,
		+1.0f, +1.0f,
		-1.0f, +1.0f
	};

	geom_vbo_.generate(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	geom_vbo_.bind();
	geom_vbo_.data(vertices, sizeof(vertices));

	color_circle_prog_ = Program::create("shaders/color_pick_hsv_circle.vert", "shaders/color_pick_hsv_circle.frag");
	color_select_prog_ = Program::create("shaders/color_pick_selected.vert", "shaders/color_pick_selected.frag");
	gradient_prog_ = Program::create("shaders/color_pick_gradient.vert", "shaders/color_pick_gradient.frag");

	color_cursor_.begin(GL_LINES);
	color_cursor_.color(0, 0, 0);
	color_cursor_.circle(0, 0, 25.0f, 32);
	color_cursor_.color(1, 1, 1);
	color_cursor_.circle(0, 0, 20.0f, 32);
	color_cursor_.end();

	text_.loadFont("menlo14");

	poses_.v().enabled(true);
	poses_.v().closeFn([&](const Leap::Frame& c){ 
		chooseState(m_leap_cursor.x, m_leap_cursor.y);
	});
}

void ColorPickController::clearCallbacks()
{
	callbacks_.clear();
}

void ColorPickController::addCallback(std::function<void(const Color&)> callback)
{
	callbacks_.push_back(callback);
}

void ColorPickController::gainFocus()
{
}

void ColorPickController::loseFocus()
{
	poses_.v().tracking(false);
}

bool ColorPickController::mouseMotion(GLFWwindow* window, double x, double y)
{
	updateState(x, y);
	return false;
}

void ColorPickController::chooseState(float x, float y)
{
	state_ = State::idle;

	if ((Vec2(x, y) - circle_rect_.center()).length() < circle_rect_.width / 2) {
		state_ = State::choose_color;
	} else if (alpha_rect_.contains(x, y)) {
		state_ = State::choose_alpha;
	} else if (value_rect_.contains(x, y)) {
		state_ = State::choose_value;
	} else if (select_rect_.contains(x, y)) {
		m_leap_cursor.x = -100;
		m_leap_cursor.y = -100;
		state_ = State::idle;
		for (std::function<void(const Color&)>& cb : callbacks_)
			cb(color_);
		callbacks_.clear();
	}
}

void ColorPickController::updateState(float x, float y)
{
	switch (state_)
	{
	case State::choose_color:
		{
			Vec2 p = Vec2(x, y) - circle_rect_.center();
			float r = min(1.0f, p.length() / circle_rect_.width * 2);
			color_.hue(p.anglePositive());
			color_.saturation(r);
			break;
		}
	case State::choose_alpha:
		{
			float alpha = max(0.0f, min(1.0f, (static_cast<float>(y)-alpha_rect_.bottom()) / alpha_rect_.height));
			color_.alpha(alpha);
			break;
		}
	case State::choose_value:
		{
			float value = max(0.0f, min(1.0f, (static_cast<float>(y)-value_rect_.bottom()) / value_rect_.height));
			color_.value(value);
			break;
		}
	}
}

bool ColorPickController::mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y)
{ 
	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
		state_ = State::idle;
	} else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) { 
		chooseState(x, y);
		if (state_ != State::idle) {
			mouseMotion(window, x, y);
		}
	}

	return false;
}

bool ColorPickController::leapInput(const Leap::Controller& leapController, const Leap::Frame& frame)
{
	poses_.v().update(frame);
	if (poses_.v().tracking()) {
		Vector v = poses_.v().handPosition(true);
		v = frame.interactionBox().normalizePoint(v);
		m_leap_cursor.x = viewport_.x + viewport_.width * v.x;
		m_leap_cursor.y = viewport_.y + viewport_.height * v.y;

		if (poses_.v().isClosed()) {
			updateState(m_leap_cursor.x, m_leap_cursor.y);
		} else {
			state_ = State::idle;
		}
	}

	return false;
}

void ColorPickController::resize()
{
	projection_ = viewport_.orthoProjection();

	float circleRadius = min(viewport_.width, viewport_.height) * 0.5f * 0.75f;
	float barSize = 0.04 * min(viewport_.width, viewport_.height);

	circle_rect_.x = viewport_.center().x - circleRadius;
	circle_rect_.y = viewport_.center().y - circleRadius + barSize * 2;
	circle_rect_.width = circleRadius * 2.0f;
	circle_rect_.height = circleRadius * 2.0f;

	alpha_rect_.x = circle_rect_.left() - barSize * 5;
	alpha_rect_.y = circle_rect_.bottom();
	alpha_rect_.width = barSize;
	alpha_rect_.height = circle_rect_.height;

	value_rect_.x = circle_rect_.right() + barSize * 4;
	value_rect_.y = circle_rect_.bottom();
	value_rect_.width = barSize;
	value_rect_.height = circle_rect_.height;

	select_rect_.x = circle_rect_.x + circle_rect_.width / 4;
	select_rect_.y = circle_rect_.y - barSize * 4;
	select_rect_.width = circle_rect_.width / 2;
	select_rect_.height = barSize * 3;

	updateText();
}

void ColorPickController::updateText()
{
	text_.clear();
	text_.viewport(viewport_);
	text_.hAlign(TextRenderer::HAlign::center);
	text_.vAlign(TextRenderer::VAlign::center);
	text_.color(1.0f, 1.0f, 1.0f);

	stringstream ss;
	ss << "Opacity: " << fixed << setprecision(2) << color_.alpha();
	text_.add(ss.str(), alpha_rect_.center().x, alpha_rect_.bottom() - viewport_.y - 24);
	
	ss.str("");
	ss << "Brightness: " << color_.value();
	text_.add(ss.str(), value_rect_.center().x, alpha_rect_.bottom() - viewport_.y - 24);
}

void ColorPickController::draw()
{
	float offset = min(viewport_.width, viewport_.height) * 0.1f;
	float size = offset * 0.25f;
	Vec2 c(viewport_.width / 2.0f, viewport_.height / 2.0f);

	geom_vbo_.bind();

	// vs_position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);

	gradient_prog_.enable();

	// background
	gradient_prog_.uniform("color1", 0.15f, 0.15f, 0.15f, 0.85f);
	gradient_prog_.uniform("color2", 0.15f, 0.15f, 0.15f, 0.85f);
	glEnable(GL_BLEND);
	quad(gradient_prog_, { (float)viewport_.x, (float)viewport_.y, (float)viewport_.width, (float)viewport_.height });
	glDisable(GL_BLEND);

	// alpha bar and knob
	gradient_prog_.uniform("color1", 1.0f, 1.0f, 1.0f, 1.0f);
	gradient_prog_.uniform("color2", 0.0f, 0.0f, 0.0f, 0.0f);
	quad(gradient_prog_, alpha_rect_);
	float x = alpha_rect_.center().x - alpha_rect_.width * 1.6f / 2;
	float y = alpha_rect_.bottom() + alpha_rect_.height * color_.alpha() - 1.5f;
	quad(gradient_prog_, { x, y, alpha_rect_.width * 1.6f, 3 });

	// value bar and knob
	gradient_prog_.uniform("color1", color_.hsv().value(1.0f).rgb().vec4());
	quad(gradient_prog_, value_rect_);
	x = value_rect_.center().x - value_rect_.width * 1.6f / 2;
	y = value_rect_.bottom() + value_rect_.height * color_.value() - 1.5f;
	quad(gradient_prog_, { x, y, value_rect_.width * 1.6f, 3 });

	// color circle
	color_circle_prog_.enable();
	color_circle_prog_.uniform("value", color_.value());
	glEnable(GL_BLEND);
	quad(color_circle_prog_, circle_rect_);
	glDisable(GL_BLEND);

	// color select box
	color_select_prog_.enable();
	color_select_prog_.uniform("color", color_.rgb().vec4());
	quad(color_select_prog_, select_rect_);

	// draw cursor circle
	x = cos(color_.hue()) * circle_rect_.width / 2.0f * color_.saturation() + circle_rect_.center().x;
	y = sin(color_.hue()) * circle_rect_.width / 2.0f * color_.saturation() + circle_rect_.center().y;
	color_cursor_.setModelViewProj(projection_ * translation(x, y, 0));
	color_cursor_.draw();

	// draw leap cursor
	if (poses_.v().tracking()) {
		color_cursor_.setModelViewProj(projection_ * translation(m_leap_cursor.x, m_leap_cursor.y, 0));
		color_cursor_.draw();
	}

	text_.draw();
}

void ColorPickController::quad(Program prog, const Rectangle<float>& rect)
{
	Mat4 t = translation(rect.center().x, rect.center().y, 0);
	Mat4 s = scale(rect.width*0.5f, rect.height*0.5f, 1);
	Mat4 mvp = projection_ * t * s;
	glUniformMatrix4fv(prog.getUniform("modelViewProjection"), 1, false, mvp);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}