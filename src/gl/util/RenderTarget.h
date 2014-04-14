#ifndef __CGL_UTIL_RENDERTARGET_H__
#define __CGL_UTIL_RENDERTARGET_H__

#include "gl/glew.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include "gl/Framebuffer.h"
#include "gl/Renderbuffer.h"

namespace gl
{
	class RenderTarget
	{
	public:
		RenderTarget();
		void clear();
		void setColorFormat(GLenum format);
		void setColorType(GLenum type);
		void setDepthFormat(GLenum format);
		const gl::Texture& getColorTarget();
		void generate(GLsizei width, GLsizei height, bool useDepth = true);
		void resize(GLsizei width, GLsizei height);
		void bind();
		void unbind();

	private:
		gl::Framebuffer framebuffer;
		gl::Texture colorTarget;
		gl::Renderbuffer depthTarget;
		GLenum colorFormat;
		GLenum colorType;
		GLenum depthFormat;
		bool useDepth;
	};
}

#endif