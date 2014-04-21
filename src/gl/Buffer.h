#ifndef __GL_BUFFER_H__
#define __GL_BUFFER_H__

#include "gl/glew.h"
#include <memory>

namespace gl
{
    /** Pointer to an OpenGL buffer object. */
    class Buffer
    {
    public:
		/** Constructs an empty buffer pointer */
		Buffer();
       
		/** Returns the handle to the OpenGL resource, or 0 if none. */
		GLuint id() const;

		/** Creates a new OpenGL resource. This object will point to it. */
		void generate(GLenum target, GLenum usage);

		/** Creates a new OpenGL resource. Sets target to GL_ARRAY_BUFFER. */
		void generateVBO(GLenum usage);

		/** Creates a new OpenGL resource. Sets target to GL_ELEMENT_ARRAY_BUFFER. */
		void generateIBO(GLenum usage);

		/** Clears this pointer. If no other objects point to the OpenGL resource, it will be destroyed. */
		void release();
        
        /** Binds the buffer to its current target. */
        void bind() const;
        
        /** Clears the binding between this buffer's target and any buffer */
        void unbind() const;
        
        /** Sets the target to which the buffer is bound. Must be applied before a call to bind() */
        void target(GLenum target);
        
        /** Sets the expected usage pattern. Must be applied before a call to setData() */
        void usage(GLenum usage);
        
        /** Uploads data to the OpenGL-managed buffer */
        void data(const GLvoid* data, GLsizeiptr size);
        
        /** Uploads data to a subset of the OpenGL-managed buffer */
        void subData(const GLvoid* data, GLsizeiptr size, GLintptr offset);

    private:
		std::shared_ptr<GLuint> handle_;
        GLenum target_;
        GLenum usage_;
    };
    
}

#endif // __GL_BUFFER_H__