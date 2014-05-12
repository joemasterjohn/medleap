#include "ColorPickController.h"
#include <algorithm>

using namespace std;
using namespace gl;
using namespace Leap;

ColorPickController::ColorPickController() :
		prev_color_(0.0f, 1.0f, 1.0f, 1.0f),
		color_(0.0f, 1.0f, 1.0f, 1.0f),
		state_(State::idle)
{
	l_pose_.engageFunction(
		std::bind(&ColorPickController::leapChooseWidget, this, std::placeholders::_1));

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
	color_cursor_.circle(0, 0, 20.0f, 32);
	color_cursor_.color(1, 1, 1);
	color_cursor_.circle(0, 0, 21.0f, 32);
	color_cursor_.end();

	text_.loadFont("menlo14");
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
	l_pose_.tracking(false);
}

bool ColorPickController::mouseMotion(GLFWwindow* window, double x, double y)
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

	return false;
}

bool ColorPickController::mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y)
{ 
	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
		state_ = State::idle;
	} else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) { 
		if ((Vec2(x, y) - circle_rect_.center()).length() < circle_rect_.width / 2) {
			state_ = State::choose_color;
			mouseMotion(window, x, y);
		}
		else if (alpha_rect_.contains(x, y)) {
			state_ = State::choose_alpha;
			mouseMotion(window, x, y);
		}
		else if (value_rect_.contains(x, y)) {
			state_ = State::choose_value;
			mouseMotion(window, x, y);
		}
		else if (select_rect_.contains(x, y)) {
			for (std::function<void(const Color&)>& cb : callbacks_)
				cb(color_);
			callbacks_.clear();
		}
	}

	return false;
}

bool ColorPickController::leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
{
	l_pose_.update(leapController);

	// DEBUG ONLY: REMOVE (change to highlight text indicating which funciton will be used)
	if (!l_pose_.tracking() && currentFrame.fingers().count() > 0) {
		Vector v = leapController.frame().fingers().frontmost().tipPosition();
		m_leap_cursor.x = v.x / 100.0f * viewport_.width / 2.0f + viewport_.center().x;
		m_leap_cursor.y = (v.y - 250) / 100.0f * viewport_.height / 2.0f + viewport_.center().y;
	} else {
		m_leap_cursor = { -200, -200 };
	}

	if (!l_pose_.tracking() && currentFrame.fingers().count() > 4) {
		for (std::function<void(const Color&)>& cb : callbacks_)
			cb(color_);
		callbacks_.clear();
	}

	return false;
}

void ColorPickController::leapChooseWidget(const Leap::Controller& controller)
{
	Vector v = controller.frame().fingers().frontmost().tipPosition();
	float x = v.x / 100.0f * viewport_.width / 2.0f + viewport_.center().x;
	float y = (v.y - 250) / 100.0f * viewport_.height / 2.0f + viewport_.center().y;

	float vpw = viewport_.width;

	if (x <= vpw / 3.0f) {
		l_pose_.trackFunction(std::bind(&ColorPickController::leapUpdateAlpha, this, std::placeholders::_1));
	}
	else if (x <= vpw / 3.0f * 2.0f) {
		l_pose_.trackFunction(std::bind(&ColorPickController::leapUpdateColor, this, std::placeholders::_1));
	}
	else {
		l_pose_.trackFunction(std::bind(&ColorPickController::leapUpdateValue, this, std::placeholders::_1));
	}

	prev_color_ = color_;
}

void ColorPickController::leapUpdateColor(const Leap::Controller& controller)
{
	Vec3 tip(l_pose_.deltaTipPos().x, l_pose_.deltaTipPos().y, l_pose_.deltaTipPos().z);

	float prevValue = 0.0f;
	float hue = Vec2(tip.x, tip.y).anglePositive();
	float sat = Vec2(tip.x, tip.y).length() / 50.0f;
	sat = max(min(sat, 1.0f), 0.0f);
	color_.hue(hue);
	color_.saturation(sat);
}

void ColorPickController::leapUpdateAlpha(const Leap::Controller& controller)
{
	Vec3 tip(l_pose_.deltaTipPos().x, l_pose_.deltaTipPos().y, l_pose_.deltaTipPos().z);
	color_.alpha(clamp(prev_color_.alpha() + tip.y / 100.0f, 0.0f, 1.0f));
}

void ColorPickController::leapUpdateValue(const Leap::Controller& controller)
{
	Vec3 tip(l_pose_.deltaTipPos().x, l_pose_.deltaTipPos().y, l_pose_.deltaTipPos().z);
	color_.value(clamp(prev_color_.value() + tip.y / 100.0f, 0.0f, 1.0f));
}

void ColorPickController::resize()
{
	projection_ = viewport_.orthoProjection();

	float circleRadius = min(viewport_.width, viewport_.height) * 0.5f * 0.75f;
	circle_rect_.x = viewport_.center().x - circleRadius;
	circle_rect_.y = viewport_.center().y - circleRadius;
	circle_rect_.width = circleRadius * 2.0f;
	circle_rect_.height = circleRadius * 2.0f;

	float barSize = 0.02 * min(viewport_.width, viewport_.height);

	alpha_rect_.x = circle_rect_.left() - barSize * 5;
	alpha_rect_.y = circle_rect_.bottom();
	alpha_rect_.width = barSize;
	alpha_rect_.height = circle_rect_.height;

	value_rect_.x = circle_rect_.right() + barSize * 4;
	value_rect_.y = circle_rect_.bottom();
	value_rect_.width = barSize;
	value_rect_.height = circle_rect_.height;

	select_rect_.x = circle_rect_.x + circle_rect_.width / 4;
	select_rect_.y = circle_rect_.y - barSize * 5;
	select_rect_.width = circle_rect_.width / 2;
	select_rect_.height = barSize * 3;
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
	float x = alpha_rect_.center().x - alpha_rect_.width * 1.3f / 2;
	float y = alpha_rect_.bottom() + alpha_rect_.height * color_.alpha() - 1.5f;
	quad(gradient_prog_, { x, y, alpha_rect_.width * 1.3f, 3 });

	// value bar and knob
	gradient_prog_.uniform("color1", color_.hsv().value(1.0f).rgb().vec4());
	quad(gradient_prog_, value_rect_);
	x = value_rect_.center().x - value_rect_.width * 1.3f / 2;
	y = value_rect_.bottom() + value_rect_.height * color_.value() - 1.5f;
	quad(gradient_prog_, { x, y, value_rect_.width * 1.3f, 3 });

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
	color_cursor_.setModelViewProj(projection_ * translation(m_leap_cursor.x, m_leap_cursor.y, 0));
	color_cursor_.draw();

	// text
	text_.begin(viewport_.width, viewport_.height);
	text_.setColor(1, 1, 1);

	stringstream ss;
	ss << "Opacity: " << fixed << setprecision(2) << color_.alpha();
	text_.add(ss.str(), alpha_rect_.center().x, alpha_rect_.bottom() - viewport_.y - 24, TextRenderer::CENTER, TextRenderer::CENTER);
	ss.str("");
	ss << "Brightness: " << color_.value();
	text_.add(ss.str(), value_rect_.center().x, alpha_rect_.bottom() - viewport_.y - 24, TextRenderer::CENTER, TextRenderer::CENTER);
	text_.end();
}

void ColorPickController::quad(Program prog, const Rectangle<float>& rect)
{
	Mat4 t = translation(rect.center().x, rect.center().y, 0);
	Mat4 s = scale(rect.width*0.5f, rect.height*0.5f, 1);
	Mat4 mvp = projection_ * t * s;
	glUniformMatrix4fv(prog.getUniform("modelViewProjection"), 1, false, mvp);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}