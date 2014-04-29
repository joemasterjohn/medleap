#ifndef __GL_UTIL_GEOMETRY_H__
#define __GL_UTIL_GEOMETRY_H__

#include "gl/glew.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include "gl/Framebuffer.h"
#include "gl/Renderbuffer.h"

namespace gl
{
	struct Geometry
	{
		std::vector<Vec3> vertices;
		std::vector<int> indices;
		GLenum mode;

		Geometry();
		Geometry(Geometry&& mesh);
		Geometry& operator=(Geometry&& mesh);
	};
}

#endif