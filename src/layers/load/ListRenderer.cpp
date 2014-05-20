#include "ListRenderer.h"
#include "util/Util.h"

using namespace gl;
using namespace std;

ListRenderer::ListRenderer() : 
	horizontal_pad_(40.0f),
	vertical_pad_(10.0f),
	item_height_(120.0f),
	index_count_(0),
	alpha_(1.0f)
{
	prog_ = Program::create("shaders/menu.vert", "shaders/menu.frag");
	vbo_.generateVBO(GL_DYNAMIC_DRAW);
	ibo_.generateIBO(GL_DYNAMIC_DRAW);

	text_.loadFont("menlo24");
}

void ListRenderer::update(const DirectoryMenu& menu, const gl::Viewport& viewport)
{
	if (menu.items().empty()) {
		index_count_ = 0;
		return;
	}

	float left = horizontal_pad_;
	float right = viewport.width - horizontal_pad_;
	float top = viewport.height - vertical_pad_;
	float bottom = top - item_height_;
	float vertical_step = item_height_ + vertical_pad_;

	vector<Vec2> vertices;
	vector<GLushort> indices;
	for (int i = 0; i < menu.items().size(); i++) {
		vertices.push_back({ left, top });
		vertices.push_back({ left, bottom });
		vertices.push_back({ right, bottom });
		vertices.push_back({ right, top });

		GLushort j = static_cast<GLushort>(4 * i);
		indices.push_back(static_cast<GLushort>(j));
		indices.push_back(static_cast<GLushort>(j + 1));
		indices.push_back(static_cast<GLushort>(j + 2));
		indices.push_back(static_cast<GLushort>(j));
		indices.push_back(static_cast<GLushort>(j + 2));
		indices.push_back(static_cast<GLushort>(j + 3));

		top -= vertical_step;
		bottom -= vertical_step;
	}
	content_height_ = menu.items().size() * (vertical_step) + vertical_pad_;

	index_count_ = indices.size();
	indices_per_item_ = index_count_ / menu.items().size();

	vbo_.bind();
	vbo_.data(&vertices[0], vertices.size() * sizeof(Vec3));

	ibo_.bind();
	ibo_.data(&indices[0], indices.size() * sizeof(GLushort));

	projection_ = viewport.orthoProjection();

	text_.clear();
	text_.color(1.0f, 1.0f, 1.0f, 1.0f);
	text_.viewport(viewport);
	text_.hAlign(TextRenderer::HAlign::center);
	text_.vAlign(TextRenderer::VAlign::center);
	float x = viewport.width / 2.0f;
	float y = viewport.height - vertical_pad_ - 0.5f * item_height_;
	for (const MenuItem& item : menu.items()) {
		text_.add(item.getName(), x, y);
		y -= item_height_ + vertical_pad_;
	}
}

void ListRenderer::draw()
{
	if (index_count_ == 0) {
		return;
	}

	drawBoxes();
	text_.model(model_);
	text_.draw();
}

void ListRenderer::drawBoxes()
{
	prog_.enable();
	prog_.uniform("modelViewProjection", projection_ * model_);
	prog_.uniform("color", .1f, .1f, .1f, alpha_);

	vbo_.bind();
	ibo_.bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_SHORT, 0);

	if (highlighted_ >= 0) {
		prog_.uniform("color", .2f, 0.3f, .2f, alpha_);
		GLvoid* offset = (GLvoid*)(indices_per_item_ * highlighted_ * sizeof(GLushort));
		glDrawElements(GL_TRIANGLES, indices_per_item_, GL_UNSIGNED_SHORT, offset);
	}
	glDisable(GL_BLEND);
}