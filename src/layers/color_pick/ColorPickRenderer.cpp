#include "ColorPickRenderer.h"
#include <vector>
#include "Color.h"
#include <sstream>

using namespace std;
using namespace gl;

ColorPickRenderer::ColorPickRenderer() : mColor(0.0f, 1.0f, 1.0f, 1.0f), mTracking(false)
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
	geomVBO.data(vertices, sizeof(vertices));

	circleShader = Program::create("shaders/color_pick_hsv_circle.vert", "shaders/color_pick_hsv_circle.frag");
	selectShader = Program::create("shaders/color_pick_selected.vert", "shaders/color_pick_selected.frag");
	gradientShader = Program::create("shaders/color_pick_gradient.vert", "shaders/color_pick_gradient.frag");

	mCursor.begin(GL_LINES);
	mCursor.color(0, 0, 0);
	mCursor.circle(0, 0, 20.0f, 32);
	mCursor.color(1, 1, 1);
	mCursor.circle(0, 0, 21.0f, 32);
	mCursor.end();

	text.loadFont("menlo14");
}

void ColorPickRenderer::draw()
{
	float offset = min(viewport.width, viewport.height) * 0.1f;
	float size = offset * 0.25f;
	Vec2 c(viewport.width / 2.0f, viewport.height / 2.0f);

	geomVBO.bind();

	// background
	{
		gradientShader.enable();
		GLint loc = gradientShader.getAttribute("vs_position");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 2, GL_FLOAT, false, 0, 0);

		// background
		glEnable(GL_BLEND);
		glUniform4f(gradientShader.getUniform("color1"), 0.15f, 0.15f, 0.15f, 0.85f);
		glUniform4f(gradientShader.getUniform("color2"), 0.15f, 0.15f, 0.15f, 0.85f);
		quad(gradientShader, {(float)viewport.x, (float)viewport.y, (float)viewport.width, (float)viewport.height});
		glDisable(GL_BLEND);


		// alpha / value bars
		glUniform4f(gradientShader.getUniform("color1"), 1.0f, 1.0f, 1.0f, 1.0f);
		glUniform4f(gradientShader.getUniform("color2"), 0.0f, 0.0f, 0.0f, 0.0f);
		quad(gradientShader, mAlphaRect);
		glUniform4fv(gradientShader.getUniform("color1"), 1, mColor.hsv().value(1.0f).rgb().vec4());
		glUniform4f(gradientShader.getUniform("color2"), 0.0f, 0.0f, 0.0f, 0.0f);
		quad(gradientShader, mValueRect);

		// bar knobs
		glUniform4f(gradientShader.getUniform("color1"), 1.0f, 1.0f, 1.0f, 1.0f);
		glUniform4f(gradientShader.getUniform("color2"), 0.0f, 0.0f, 0.0f, 0.0f);
		float x = mAlphaRect.center().x - mAlphaRect.width * 1.3f / 2;
		float y = mAlphaRect.bottom() + mAlphaRect.height * mColor.alpha() - 1.5f;
		quad(gradientShader, { x, y, mAlphaRect.width * 1.3f, 3 });
		x = mValueRect.center().x - mValueRect.width * 1.3f / 2;
		y = mValueRect.bottom() + mValueRect.height * mColor.value() - 1.5f;
		quad(gradientShader, { x, y, mValueRect.width * 1.3f, 3 });
	}

	// color circle
	{
		circleShader.enable();
		GLint loc = circleShader.getAttribute("vs_position");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 2, GL_FLOAT, false, 0, 0);

		glUniform1f(circleShader.getUniform("value"), mColor.value());

		glEnable(GL_BLEND);
		quad(circleShader, mCircleRect);
		glDisable(GL_BLEND);
	}

	// color select box
	{
		selectShader.enable();
		glUniform4fv(selectShader.getUniform("color"), 1, mColor.rgb().vec4());
		GLint loc = selectShader.getAttribute("vs_position");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 2, GL_FLOAT, false, 0, 0);

		quad(selectShader, mPreviewRect);
	}

	// draw cursor circle
	float x = cos(mColor.hue()) * mCircleRect.width / 2.0f * mColor.saturation() + mCircleRect.center().x;
	float y = sin(mColor.hue()) * mCircleRect.width / 2.0f * mColor.saturation() + mCircleRect.center().y;
	mCursor.setModelViewProj(mProjection * translation(x,y,0));
	mCursor.draw();

	// draw leap cursor
	mCursor.setModelViewProj(mProjection * translation(m_leap_cursor.x, m_leap_cursor.y, 0));
	mCursor.draw();

	text.begin(viewport.width, viewport.height);
	if (mTracking)
		text.setColor(0.5f, 0.5f, 1);
	else
		text.setColor(1, 1, 1);

	stringstream ss;
	ss << "Opacity: " << fixed << setprecision(2) << mColor.alpha();
	text.add(ss.str(), mAlphaRect.center().x, mAlphaRect.bottom()-viewport.y-24, TextRenderer::CENTER, TextRenderer::CENTER);
	ss.str("");
	ss << "Brightness: " << mColor.value();
	text.add(ss.str(), mValueRect.center().x, mAlphaRect.bottom() - viewport.y - 24, TextRenderer::CENTER, TextRenderer::CENTER);
	text.end();
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

	float barSize = 0.02 * min(viewport.width, viewport.height);

	mAlphaRect.x = mCircleRect.left() - barSize * 5;
	mAlphaRect.y = mCircleRect.bottom();
	mAlphaRect.width = barSize;
	mAlphaRect.height = mCircleRect.height;

	mValueRect.x = mCircleRect.right() + barSize * 4;
	mValueRect.y = mCircleRect.bottom();
	mValueRect.width = barSize;
	mValueRect.height = mCircleRect.height;

	mPreviewRect.x = mCircleRect.x + mCircleRect.width / 4;
	mPreviewRect.y = mCircleRect.y - barSize * 5;
	mPreviewRect.width = mCircleRect.width / 2;
	mPreviewRect.height = barSize * 3;
}