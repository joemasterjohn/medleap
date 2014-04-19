#include "OrientationRenderer.h"
#include "gl/geom/Box.h"
#include "main/MainController.h"
#include "gl/math/Transform.h"
using namespace gl;

OrientationRenderer::OrientationRenderer() : camera(NULL)
{
	geomVBO.generate(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	geomIBO.generate(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);

	Box box{ 1.0f };
	box.fill(geomVBO, geomIBO);
	numElements = box.getIndices().size();

	geomShader = Program::create("shaders/menu.vert", "shaders/menu.frag");
}

void OrientationRenderer::draw()
{

	Mat3 rot;
	if (MainController::getInstance().getMode() == MainController::MODE_2D) {
		Mat3 img2gl{ 1, 0, 0, 0, -1, 0, 0, 0, -1 };
		rot = img2gl * volume->getPatientBasis().transpose();
	}
	else {
		Mat3 img2gl{ 1, 0, 0, 0, -1, 0, 0, 0, -1 };
		const Mat4& view = camera->getView();
		rot = Mat3{ view.col(0), view.col(1), view.col(2) } * img2gl * volume->getPatientBasis().transpose();
	}

	int draw_w = std::min(viewport.width,viewport.height) * 0.15;
	int draw_h = draw_w;
	int draw_x = viewport.width - draw_w;
	int draw_y = viewport.height - draw_h;

	Mat4 mvp{ 
		Vec4(rot.col(0).x, rot.col(0).y, rot.col(0).z, 0), 
		Vec4(rot.col(1).x, rot.col(1).y, rot.col(1).z, 0),
		Vec4(rot.col(2).x, rot.col(2).y, rot.col(2).z, 0),
		Vec4(0, 0, 0, 1) };

	glViewport(draw_x, draw_y, draw_w, draw_h);

	geomShader.enable();
	glUniformMatrix4fv(geomShader.getUniform("modelViewProjection"), 1, false,  mvp);
	glUniform4f(geomShader.getUniform("color"), 0.5f, 0.5f, 0.5f, 0.5f);

	geomVBO.bind();
	geomIBO.bind();

	GLint loc = geomShader.getAttribute("vs_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, false, 0, 0);

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);

	TextRenderer& tr = MainController::getInstance().getText();
	Vec3 c = MainController::getInstance().getRenderer().getInverseBGColor();
	tr.setColor(c.x, c.y, c.z);
	tr.begin(draw_w, draw_h);

	auto text = [&](const Vec4& p, const std::string& label){
		// convert 3D position p to viewport coordinates x, y
		Vec4 ndc = mvp * p;
		ndc /= ndc.w;
		float x = (ndc.x + 1.0f) * draw_w / 2.0f;
		float y = (ndc.y + 1.0f) * draw_h / 2.0f;
		if (ndc.z > -0.01)
			tr.add(label, x, y, TextRenderer::CENTER, TextRenderer::CENTER);
	};


	if (volume->getModality() == VolumeData::UNKNOWN) {
		text(Vec4(+0.6f, 0.0f, 0.0f, 1.0f), "+X");
		text(Vec4(-0.6f, 0.0f, 0.0f, 1.0f), "-X");
		text(Vec4(0.0f, +0.6f, 0.0f, 1.0f), "+Y");
		text(Vec4(0.0f, -0.6f, 0.0f, 1.0f), "-Y");
		text(Vec4(0.0f, 0.0f, +0.6f, 1.0f), "+Z");
		text(Vec4(0.0f, 0.0f, -0.6f, 1.0f), "-Z");
	}
	else {
		text(Vec4(+0.6f, 0.0f, 0.0f, 1.0f), "L");
		text(Vec4(-0.6f, 0.0f, 0.0f, 1.0f), "R");
		text(Vec4(0.0f, +0.6f, 0.0f, 1.0f), "P");
		text(Vec4(0.0f, -0.6f, 0.0f, 1.0f), "A");
		text(Vec4(0.0f, 0.0f, +0.6f, 1.0f), "S");
		text(Vec4(0.0f, 0.0f, -0.6f, 1.0f), "I");
	}


	tr.end();
}


void OrientationRenderer::resize(int width, int height)
{
}