#ifndef CGL_UTIL_DRAW_H_
#define CGL_UTIL_DRAW_H_

#include "gl/glew.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "gl/math/Vector3.h"
#include "gl/math/Matrix4.h"
#include "Geometry.h"
#include <vector>

/** Simple "immediate mode" utility for debug drawing */
namespace gl{
	class Draw
	{
	public:
		Draw();

		void begin(GLenum mode);
		void end();
		void draw();

		void setModelViewProj(const Mat4& mvp);

		void color(float r, float g, float b);

		void vertex(float x, float y, float z = 0.0f);

		void line(float x1, float y1, float x2, float y2);

		void circle(float x, float y, float radius, int numSegments);

		void geometry(const Geometry& g);

	private:
		Mat4 modelViewProjection;
		gl::Program lineShader;
		gl::Buffer lineBuffer;
		std::vector<GLfloat> vertices;
		Vec3 currentColor;
		GLenum mode;
	};
}

#endif // CGL_UTIL_DRAW_H_