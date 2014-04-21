#ifndef __GL_RENDERBUFFER_H__
#define __GL_RENDERBUFFER_H__

#include "gl/glew.h"
#include <memory>

namespace gl
{
	/** Pointer to an OpenGL renderbuffer object */
    class Renderbuffer
    {
	public:
		Renderbuffer();

		/** Returns the handle to the OpenGL resource, or 0 if none. */
		GLuint id() const;

		/** Creates a new OpenGL resource. This object will point to it. */
		void generate();

		/** Clears this pointer. If no other objects point to the OpenGL resource, it will be destroyed. */
		void release();

		void bind() const;

		void unbind() const;

		void storage(GLenum internalFormat, GLsizei width, GLsizei height);

		void storage(GLenum internalFormat, GLsizei width, GLsizei height, GLsizei samples);

	private:
		std::shared_ptr<GLuint> handle_;
    };
    
}

#endif // __GL_RENDERBUFFER_H__