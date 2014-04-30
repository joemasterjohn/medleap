#include "LeapStateController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"
#include "util/stb_image.h"

using namespace gl;
using namespace std;

static const int max_states = 5;

LeapStateController::LeapStateController() : active_state_(nullptr)
{
	state_views_.resize(max_states, { string(), Texture(), true });

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
	load(state_views_[(int)State::h1f1_point], "Open Hand", "icons/h1f1_point.png");
	load(state_views_[(int)State::h1f2_point], "Open Hand", "icons/h1f2_point.png");
	load(state_views_[(int)State::h1f2_trigger], "Open Hand", "icons/h1f2_trigger.png");
	load(state_views_[(int)State::h1f5_spread], "Open Hand", "icons/h1f5_spread.png");
	load(state_views_[(int)State::h2f1_point], "Open Hand", "icons/h2f1_point.png");

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

void LeapStateController::availableStates(set<State> states)
{
	for (int i = 0; i < max_states; i++) {
		bool available = states.find((State)i) != states.end();
		state_views_[i].available_ = available;
	}
}

void LeapStateController::availableState(State state, bool available)
{
	state_views_[(int)state].available_ = available;
}

void LeapStateController::activeState(State state)
{
	if (state == State::none) {
		active_state_ = nullptr;
	} else {
		active_state_ = &state_views_[(int)state];
	}
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

	Vec4 active_color, inactive_color;
	if (MainController::getInstance().getRenderer().getBackgroundColor().x > 0.5f) {
		active_color   = { 0.0f, 0.0f, 0.0f, 1.0f };
		inactive_color = { 0.75f, 0.75f, 0.75f, 1.0f };
	} else {
		active_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		inactive_color = { 0.25f, 0.25f, 0.25f, 1.0f };
	}

	Mat4 m_scale = scale(1.0f, -s, 1.0f);
	for (int i = 0; i < max_states; i++) {
		StateView& view = state_views_[i];

		if (view.available_) {
			view.texture_.bind();
			float offset = s + i * s * 2;
			icon_prog_.uniform("model_view", translation(0, 1.0f - offset, 0) * m_scale);
			if (&view == active_state_) {
				icon_prog_.uniform("color", active_color);
			} else {
				icon_prog_.uniform("color", inactive_color);
			}
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	
	glDisable(GL_BLEND);

	TextRenderer& text = MainController::getInstance().getText();
	text.begin(viewport_.width, viewport_.height);
	text.add("Test", viewport_.width/2, 50, TextRenderer::CENTER, TextRenderer::CENTER);
	text.end();
}