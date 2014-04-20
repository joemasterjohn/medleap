#include "ColorPickRenderer.h"
#include <vector>
#include "gl/math/Vector3.h"
#include "gl/math/Math.h"
#include "gl/math/Matrix4.h"
#include "gl/math/Transform.h"
#include "Color.h"

using namespace std;
using namespace gl;

ColorPickRenderer::ColorPickRenderer() : mColor(0.0f, 1.0f, 1.0f, 1.0f)
{
	GLfloat vertices[] = {
		-1.0f, -1.0f,
		+1.0f, -1.0f,
		+1.0f, +1.0f,
		-1.0f, -1.0f,
		+1.0f, +1.0f,
		-1.0f, +1.0f
	};

	geomVBO.generate(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	geomVBO.bind();
	geomVBO.setData(vertices, sizeof(vertices));

	circleShader = Program::create("shaders/color_pick_hsv_circle.vert", "shaders/color_pick_hsv_circle.frag");
	selectShader = Program::create("shaders/color_pick_selected.vert", "shaders/color_pick_selected.frag");
	gradientShader = Program::create("shaders/color_pick_gradient.vert", "shaders/color_pick_gradient.frag");

	mCursor.begin(GL_LINES);
	mCursor.color(0, 0, 0);
	mCursor.circle(0, 0, 20.0f, 32);
	mCursor.color(1, 1, 1);
	mCursor.circle(0, 0, 21.0f, 32);
	mCursor.end();
}

void ColorPickRenderer::draw()
{
	float offset = min(viewport.width, viewport.height) * 0.1f;
	float size = offset * 0.25f;
	Vec2 c(viewport.width / 2.0f, viewport.height / 2.0f);

	geomVBO.bind();

	// color circle
	{
		circleShader.enable();
		GLint loc = circleShader.getAttribute("vs_position");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 2, GL_FLOAT, false, 0, 0);

		glEnable(GL_BLEND);
		quad(circleShader, mCircleRect);
		glDisable(GL_BLEND);
	}

	// color select box
	{
		selectShader.enable();
		glUniform4fv(selectShader.getUniform("color"), 1, mColor.rgb().vec4());
		glUniform2f(selectShader.getUniform("tiles"), 4, 4);
		GLint loc = selectShader.getAttribute("vs_position");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 2, GL_FLOAT, false, 0, 0);

		quad(selectShader, mPreviewRect);
	}

	// alpha/value bars
	{
		gradientShader.enable();
		GLint loc = gradientShader.getAttribute("vs_position");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 2, GL_FLOAT, false, 0, 0);

		quad(gradientShader, mAlphaRect);
		quad(gradientShader, mValueRect);
	}

	// draw cursor circle
	float x = cos(mColor.hue()) * mCircleRect.width / 2.0f * mColor.saturation() + mCircleRect.center().x;
	float y = sin(mColor.hue()) * mCircleRect.width / 2.0f * mColor.saturation() + mCircleRect.center().y;
	mCursor.setModelViewProj(mProjection * translation(x,y,0));
	mCursor.draw();
}

void ColorPickRenderer::quad(Program prog, const Rectangle<float>& rect)
{
	Mat4 t = translation(rect.center().x, rect.center().y, 0);
	Mat4 s = scale(rect.width*0.5f, rect.height*0.5f, 1);
	Mat4 mvp = mProjection * t * s;
	glUniformMatrix4fv(prog.getUniform("modelViewProjection"), 1, false, mvp);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void ColorPickRenderer::resize(int width, int height)
{
	mProjection = viewport.orthoProjection();

	float circleRadius = min(viewport.width, viewport.height) * 0.5f * 0.75f;
	mCircleRect.x = viewport.center().x - circleRadius;
	mCircleRect.y = viewport.center().y - circleRadius;
	mCircleRect.width = circleRadius * 2.0f;
	mCircleRect.height = circleRadius * 2.0f;

	float barSize = 0.025 * min(viewport.width, viewport.height);

	mAlphaRect.x = mCircleRect.left() - barSize * 5;
	mAlphaRect.y = mCircleRect.bottom();
	mAlphaRect.width = barSize;
	mAlphaRect.height = mCircleRect.height;

	mValueRect.x = mCircleRect.right() + barSize * 4;
	mValueRect.y = mCircleRect.bottom();
	mValueRect.width = barSize;
	mValueRect.height = mCircleRect.height;

	mPreviewRect.x = mCircleRect.x;
	mPreviewRect.y = mCircleRect.y - barSize * 5;
	mPreviewRect.width = mCircleRect.width;
	mPreviewRect.height = barSize;
}