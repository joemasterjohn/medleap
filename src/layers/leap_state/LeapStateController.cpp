#include "LeapStateController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"
#include "util/stb_image.h"

using namespace gl;
using namespace std;

LeapStateController::LeapStateController()
{
	// 5 states (careful if this changes; this is not ideal)
	state_views_.resize(5, {string(), Texture(), true});

	auto load = [](StateView& s, const std::string& label, const char* filename) {
		s.label_ = label;

		int width, height, channels;
		unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
		if (!data) {
			cerr << "WARNING: couldn't load " << filename << endl;
		} else {
			s.texture_.generate(GL_TEXTURE_2D);
			s.texture_.bind();
			s.texture_.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			s.texture_.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			s.texture_.setData2D(GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		stbi_image_free(data);
	};

	// load the state views and their textures
	load(state_views_[(int)State::h1f1_point], "Open Hand", "C:/Users/Justin/Projects/MedLeap/medleap/icons/h1f1_point.png");
	load(state_views_[(int)State::h1f2_point], "Open Hand", "C:/Users/Justin/Projects/MedLeap/medleap/icons/h1f2_point.png");
	load(state_views_[(int)State::h1f2_trigger], "Open Hand", "C:/Users/Justin/Projects/MedLeap/medleap/icons/h1f2_trigger.png");
	load(state_views_[(int)State::h1f5_spread], "Open Hand", "C:/Users/Justin/Projects/MedLeap/medleap/icons/h1f5_spread.png");
	load(state_views_[(int)State::h2f1_point], "Open Hand", "C:/Users/Justin/Projects/MedLeap/medleap/icons/h2f1_point.png");


	// quad geometry
	GLfloat vertices[] = {
		-1.0f, -1.0f, +0.0f, +0.0f,
		+1.0f, -1.0f, +1.0f, +0.01f,
		+1.0f, +1.0f, +1.0f, +1.0f,
		-1.0f, -1.0f, +0.0f, +0.0f,
		+1.0f, +1.0f, +1.0f, +1.0f,
		-1.0f, +1.0f, +0.0f, +1.0f
	};
	icon_vbo_.generateVBO(GL_STATIC_DRAW);
	icon_vbo_.bind();
	icon_vbo_.data(vertices, sizeof(vertices));

	icon_prog_ = Program::create("shaders/icon.vert", "shaders/icon.frag");
}

void LeapStateController::availableState(State state, bool available)
{
	// TODO
}

void LeapStateController::activeState(State state)
{
	// TODO
}

void LeapStateController::draw()
{
	float s = viewport_.aspect();

	icon_prog_.enable();
	icon_vbo_.bind();


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Mat4 m_scale = scale(1.0f, -s, 1.0f);
	for (int i = 0; i < 5; i++) {
		state_views_[i].texture_.bind();
		float offset = s + i * s * 2;
		icon_prog_.uniform("model_view", translation(0, 1.0f - offset, 0) * m_scale);
		icon_prog_.uniform("color", 0.5f, 0.5f, 0.5f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	
	glDisable(GL_BLEND);
}