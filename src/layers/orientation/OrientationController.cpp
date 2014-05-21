#include "OrientationController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"

using namespace gl;
using namespace std;

OrientationController::OrientationController() : 
	camera_(nullptr),
	volume_(nullptr)
{
	vector<Vec3> vertices = {
		{ -0.5f, -0.5f, +0.5f },
		{ +0.5f, -0.5f, +0.5f },
		{ +0.5f, +0.5f, +0.5f },
		{ -0.5f, +0.5f, +0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ +0.5f, -0.5f, -0.5f },
		{ +0.5f, +0.5f, -0.5f },
		{ -0.5f, +0.5f, -0.5f }
	};
	cube_vbo_.generate(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	cube_vbo_.bind();
	cube_vbo_.data(&vertices[0], vertices.size() * sizeof(Vec3));

	vector<GLushort> indices = {
		// faces
		0, 1, 2, 0, 2, 3,
		1, 5, 6, 1, 6, 2,
		5, 4, 7, 5, 7, 6,
		4, 0, 3, 4, 3, 7,
		3, 2, 6, 3, 6, 7,
		0, 4, 5, 0, 5, 1,
		// edges
		0, 1, 1, 2, 2, 3, 3, 0,
		5, 6, 6, 7, 7, 4, 4, 5,
		4, 0, 7, 3, 6, 2, 5, 1
	};
	face_index_count_ = 36;
	face_index_offset_ = 0;
	edge_index_count_ = 24;
	edge_index_offset_ = face_index_count_ * sizeof(GLushort);
	cube_ibo_.generate(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
	cube_ibo_.bind();
	cube_ibo_.data(&indices[0], indices.size() * sizeof(GLushort));

	cube_prog_ = Program::create("shaders/orientation_cube.vert", "shaders/orientation_cube.frag");

	text_.loadFont("menlo14");
}

void OrientationController::draw()
{
	if (!volume_) {
		return;
	}

	Mat4 modelView;
	if (MainController::getInstance().getMode() == MainController::MODE_2D) {
		modelView = rotationX(pi) * volume_->getPatientBasis().transpose();
	} else {
		modelView = camera_->view().rotScale() * rotationX(pi) * volume_->getPatientBasis().transpose();
	}

	Viewport cube_vp;
	cube_vp.width = std::min(viewport_.width, viewport_.height) * 0.15;
	cube_vp.height = cube_vp.width;
	cube_vp.x = viewport_.right() - cube_vp.width;
	cube_vp.y = viewport_.top() - cube_vp.height;
	cube_vp.apply();

	cube_prog_.enable();
	cube_prog_.uniform("modelViewProjection", modelView);

	cube_vbo_.bind();
	cube_ibo_.bind();

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

	Vec4 face_color;
	Vec4 edge_color;
	if (MainController::getInstance().getRenderer().getBackgroundColor().x > 0.5f) {
		face_color = Vec4(0.9f, 0.9f, 0.9f, 1.0f);
		edge_color = Vec4(0.7f, 0.7f, 0.7f, 1.0f);
	} else {
		face_color = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
		edge_color = Vec4(0.3f, 0.3f, 0.3f, 1.0f);
	}

	cube_prog_.uniform("color", face_color);
	glDrawElements(GL_TRIANGLES, face_index_count_, GL_UNSIGNED_SHORT, (GLvoid*)(face_index_offset_));

	cube_prog_.uniform("color", edge_color);
	glDrawElements(GL_LINES, edge_index_count_, GL_UNSIGNED_SHORT, (GLvoid*)(edge_index_offset_));

	Vec3 c = MainController::getInstance().getRenderer().getInverseBGColor();
	text_.clear();
	text_.color(c);
	text_.viewport(cube_vp);
	text_.hAlign(TextRenderer::HAlign::center);
	text_.vAlign(TextRenderer::VAlign::center);

	auto text = [&](const Vec4& p, const std::string& label){
		// convert 3D position p to viewport coordinates x, y
		Vec4 ndc = modelView * p;
		ndc /= ndc.w;
		float x = (ndc.x + 1.0f) * cube_vp.width * 0.5f;
		float y = (ndc.y + 1.0f) * cube_vp.height * 0.5f;
		if (ndc.z > -0.1)
			text_.add(label, x, y);
	};

	if (volume_->getModality() == VolumeData::UNKNOWN) {
		text(Vec4(+0.6f, 0.0f, 0.0f, 1.0f), "+X");
		text(Vec4(-0.6f, 0.0f, 0.0f, 1.0f), "-X");
		text(Vec4(0.0f, +0.6f, 0.0f, 1.0f), "+Y");
		text(Vec4(0.0f, -0.6f, 0.0f, 1.0f), "-Y");
		text(Vec4(0.0f, 0.0f, +0.6f, 1.0f), "+Z");
		text(Vec4(0.0f, 0.0f, -0.6f, 1.0f), "-Z");
	} else {
		text(Vec4(+0.6f, 0.0f, 0.0f, 1.0f), "L");
		text(Vec4(-0.6f, 0.0f, 0.0f, 1.0f), "R");
		text(Vec4(0.0f, +0.6f, 0.0f, 1.0f), "P");
		text(Vec4(0.0f, -0.6f, 0.0f, 1.0f), "A");
		text(Vec4(0.0f, 0.0f, +0.6f, 1.0f), "S");
		text(Vec4(0.0f, 0.0f, -0.6f, 1.0f), "I");
	}

	text_.draw();
}