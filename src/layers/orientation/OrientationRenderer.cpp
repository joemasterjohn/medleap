#include "OrientationRenderer.h"
#include "main/MainController.h"
#include "gl/math/Transform.h"

using namespace gl;
using namespace std;

OrientationRenderer::OrientationRenderer() : camera(NULL)
{	
	vector<Vec3> vertices;
	vertices.push_back({ -0.5f, -0.5f, +0.5f });
	vertices.push_back({ +0.5f, -0.5f, +0.5f });
	vertices.push_back({ +0.5f, +0.5f, +0.5f });
	vertices.push_back({ -0.5f, +0.5f, +0.5f });
	vertices.push_back({ -0.5f, -0.5f, -0.5f });
	vertices.push_back({ +0.5f, -0.5f, -0.5f });
	vertices.push_back({ +0.5f, +0.5f, -0.5f });
	vertices.push_back({ -0.5f, +0.5f, -0.5f });

	geomVBO.generate(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	geomVBO.bind();
	geomVBO.setData(&vertices[0], vertices.size() * sizeof(Vec3));


	vector<GLushort> indices;
	auto indicesPush = [&](const vector<GLushort>& v) { indices.insert(indices.end(), v.begin(), v.end()); };

	indicesPush({ 0, 1, 2,   0, 2, 3 });
	indicesPush({ 1, 5, 6,   1, 6, 2 });
	indicesPush({ 5, 4, 7,   5, 7, 6 });
	indicesPush({ 4, 0, 3,   4, 3, 7 });
	indicesPush({ 3, 2, 6,   3, 6, 7 });
	indicesPush({ 0, 4, 5,   0, 5, 1 });
	faceIndicesCount = indices.size();
	faceIndicesOffset = 0;

	indicesPush({ 0, 1,   1, 2,   2, 3,   3, 0 });
	indicesPush({ 5, 6,   6, 7,   7, 4,   4, 5 });
	indicesPush({ 4, 0,   7, 3,   6, 2,   5, 1 });
	edgeIndicesCount = indices.size() - faceIndicesCount;
	edgeIndicesOffset = faceIndicesCount * sizeof(GLushort);

	geomIBO.generate(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
	geomIBO.bind();
	geomIBO.setData(&indices[0], indices.size() * sizeof(GLushort));

	geomShader = Program::create("shaders/menu.vert", "shaders/menu.frag");
}

void OrientationRenderer::draw()
{
	Mat4 modelView;
	if (MainController::getInstance().getMode() == MainController::MODE_2D) {
		modelView = rotationX(pi) * volume->getPatientBasis().transpose();
	}
	else {
		modelView = camera->getView().rotScale() * rotationX(pi) * volume->getPatientBasis().transpose();
	}

	int draw_w = std::min(viewport.width,viewport.height) * 0.15;
	int draw_h = draw_w;
	int draw_x = viewport.width - draw_w;
	int draw_y = viewport.height - draw_h;

	glViewport(draw_x, draw_y, draw_w, draw_h);

	geomShader.enable();
	glUniformMatrix4fv(geomShader.getUniform("modelViewProjection"), 1, false, modelView);

	geomVBO.bind();
	geomIBO.bind();

	GLint loc = geomShader.getAttribute("vs_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, false, 0, 0);

	// draw box faces
	{
		Vec3 fc;
		Vec3 lc;
		if (MainController::getInstance().getRenderer().getBackgroundColor().x > 0.5f) {
			fc = Vec3(0.9f);
			lc = Vec3(0.7f);
		}
		else {
			fc = Vec3(0.1f);
			lc = Vec3(0.3f);
		}

		glUniform4f(geomShader.getUniform("color"), fc.x, fc.y, fc.z, 0.5f);
		glEnable(GL_CULL_FACE);
		glDrawElements(GL_TRIANGLES, faceIndicesCount, GL_UNSIGNED_SHORT, (GLvoid*)(faceIndicesOffset));
		glDisable(GL_CULL_FACE);

		// draw box edges
		glUniform4f(geomShader.getUniform("color"), lc.x, lc.y, lc.z, 0.5f);
		glDrawElements(GL_LINES, edgeIndicesCount, GL_UNSIGNED_SHORT, (GLvoid*)(edgeIndicesOffset));
	}


	TextRenderer& tr = MainController::getInstance().getText();
	Vec3 c = MainController::getInstance().getRenderer().getInverseBGColor();
	tr.setColor(c.x, c.y, c.z);
	tr.begin(draw_w, draw_h);

	auto text = [&](const Vec4& p, const std::string& label){
		// convert 3D position p to viewport coordinates x, y
		Vec4 ndc = modelView * p;
		ndc /= ndc.w;
		float x = (ndc.x + 1.0f) * draw_w / 2.0f;
		float y = (ndc.y + 1.0f) * draw_h / 2.0f;
		if (ndc.z > -0.1)
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