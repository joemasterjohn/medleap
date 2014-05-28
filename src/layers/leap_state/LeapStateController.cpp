#include "LeapStateController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"
#include "util/stb_image.h"

using namespace gl;
using namespace std;

LeapStateController::LeapStateController()
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
    loadIcon(icon_point_circle, "icons/point.png");
    loadIcon(icon_three_circle, "icons/three.png");
    loadIcon(icon_l_closed, "icons/l_closed.png");
	loadIcon(icon_l_open, "icons/l_open.png");
	loadIcon(icon_v_closed, "icons/v_closed.png");
	loadIcon(icon_v_open, "icons/v_open.png");
	loadIcon(icon_carry, "icons/carry.png");
	loadIcon(icon_fist, "icons/fist.png");
	loadIcon(icon_palms_face, "icons/palms_face.png");
	loadIcon(icon_pinch, "icons/pinch.png");
	loadIcon(icon_point, "icons/point.png");
	loadIcon(icon_point2, "icons/point2.png");
	loadIcon(icon_three, "icons/three.png");
	loadIcon(icon_thumb, "icons/thumb.png");

    
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
	displayed_.push_back({ icon, label, 0.0f });
}

void LeapStateController::increaseBrightness(Icon icon)
{
    for (DisplayedIcon& ic : displayed_) {
        if (ic.icon == icon) {
            ic.brightness = std::min(1.0f, ic.brightness + 0.1f);
        }
    }
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
        
        d.brightness = std::max(0.0f, d.brightness - 0.05f);
        
		textures_[d.icon].bind();
		Mat4 m_translation = translation(0, offset + (padding + viewport_.width) * rendered, 0);
		icon_prog_.uniform("model_view", m_proj * m_translation * m_scale);
        
        Vec4 displayed_color = lerp(inactive_color, active_color, d.brightness);
        icon_prog_.uniform("color", displayed_color);
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