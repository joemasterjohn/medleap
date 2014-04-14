#ifndef __CGL_UTIL_FULLSCREENQUAD_H__
#define __CGL_UTIL_FULLSCREENQUAD_H__

#include "gl/Program.h"
#include "gl/Buffer.h"
#include "gl/Texture.h"

namespace gl {
	class FullScreenQuad
	{
	public:
		FullScreenQuad();
		void generate();
		void draw(const gl::Texture& texture);

	private:
		gl::Program shaderProgram;
		gl::Buffer vertexBuffer;
	};
}

#endif