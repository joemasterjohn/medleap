#include "LeapStateController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"
#include "util/stb_image.h"

using namespace gl;
using namespace std;

LeapStateController::LeapStateController() : active_(icon_none)
{
	auto loadIcon = [&](Icon icon, const char* filename) {
		int width, height, channels;
		unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
		if (!data) {
			cerr << "WARNING: couldn't load " << filename << endl;
		} else {
			Texture& t = textures_[icon];
			t.generate(GL_TEXTURE_2D);
			t.bind();
			t.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			t.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			t.setData2D(GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		stbi_image_free(data);
	};

	textures_.resize(num_icons);
	loadIcon(icon_h1f1_point, "icons/h1f1_point.png");
	loadIcon(icon_h1f2_point, "icons/h1f2_point.png");
	loadIcon(icon_h1f3_point, "icons/h1f3_point.png");
	loadIcon(icon_h1f4_point, "icons/h1f4_point.png");
	loadIcon(icon_h1f5_point, "icons/h1f5_point.png");
	loadIcon(icon_h1f1_circle, "icons/h1f1_circle.png");
	loadIcon(icon_h1f2_circle, "icons/h1f2_circle.png");
	loadIcon(icon_h1f2_trigger, "icons/h1f2_trigger.png");
	loadIcon(icon_h2f1_point, "icons/h2f1_point.png");

	GLfloat vertices[] = {
		0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f
	};
	icon_vbo_.generateVBO(GL_STATIC_DRAW);
	icon_vbo_.bind();
	icon_vbo_.data(vertices, sizeof(vertices));

	icon_prog_ = Program::create("shaders/icon.vert", "shaders/icon.frag");

	text_.loadFont("menlo14");
}

void LeapStateController::clear()
{
	displayed_.clear();
}

void LeapStateController::add(Icon icon, const std::string& label)
{
	displayed_.push_back({ icon, label });
}

void LeapStateController::active(Icon icon)
{
	active_ = icon;
}

void LeapStateController::draw()
{
	Mat4 m_proj = viewport_.orthoProjection();
	Mat4 m_scale = scale(viewport_.width, -viewport_.width, 1);

	icon_prog_.enable();
	icon_vbo_.bind();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vec4 active_color, inactive_color;
	if (MainController::getInstance().getRenderer().getBackgroundColor().x > 0.5f) {
		active_color   = { 0.0f, 0.0f, 0.0f, 1.0f };
		inactive_color = { 0.75f, 0.75f, 0.75f, 1.0f };
	} else {
		active_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		inactive_color = { 0.25f, 0.25f, 0.25f, 1.0f };
	}

	int rendered = 1;
	int padding = 32;
	int icons_total_height = (displayed_.size() - 1) * padding + displayed_.size() * viewport_.width;
	int offset = (viewport_.height - icons_total_height) / 2.0f;
	for (DisplayedIcon& d : displayed_) {
		textures_[d.icon].bind();
		Mat4 m_translation = translation(0, offset + (padding + viewport_.width) * rendered, 0);
		icon_prog_.uniform("model_view", m_proj * m_translation * m_scale);
		if (d.icon == active_) {
			icon_prog_.uniform("color", active_color);
		} else {
			icon_prog_.uniform("color", inactive_color);
		}
		glDrawArrays(GL_TRIANGLES, 0, 6);
		rendered++;
	}
	
	glDisable(GL_BLEND);

	rendered = 0;
	text_.clear();
	text_.color(MainController::getInstance().getRenderer().getInverseBGColor());
	text_.viewport(viewport_);
	text_.hAlign(TextRenderer::HAlign::center);
	text_.vAlign(TextRenderer::VAlign::center);
	offset += padding / 2;
	for (DisplayedIcon& d : displayed_) {
		text_.add(d.label, viewport_.width / 2, offset + (padding + viewport_.width) * rendered++);
	}
	text_.draw();

	Draw& draw = MainController::getInstance().draw();
	draw.setModelViewProj(m_proj);
	draw.begin(GL_LINES);
	draw.color(0.5f, 0.5f, 0.5f);
	draw.vertex(viewport_.width, 0.0f);
	draw.vertex(viewport_.width, viewport_.height);
	draw.end();
	draw.draw();
}