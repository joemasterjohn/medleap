#ifndef CGL_BUFFER_H_
#define CGL_BUFFER_H_

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

		/** Clears this pointer. If no other objects point to the OpenGL resource, it will be destroyed. */
		void release();
        
        /** Binds the buffer to its current target. */
        void bind() const;
        
        /** Clears the binding between this buffer's target and any buffer */
        void unbind() const;
        
        /** Sets the target to which the buffer is bound. Must be applied before a call to bind() */
        void setTarget(GLenum target);
        
        /** Sets the expected usage pattern. Must be applied before a call to setData() */
        void setUsage(GLenum usage);
        
        /** Uploads data to the OpenGL-managed buffer */
        void setData(const GLvoid* data, GLsizeiptr size);
        
        /** Uploads data to a subset of the OpenGL-managed buffer */
        void setSubData(const GLvoid* data, GLsizeiptr size, GLintptr offset);
        
        /** Creates a vertex buffer (GL_ARRAY_BUFFER) with default usage GL_STATIC_DRAW */
        static Buffer genVertexBuffer(GLenum usage = GL_STATIC_DRAW);
        
        /** Creates an index buffer (GL_ELEMENT_ARRAY_BUFFER) with default usage GL_STATIC_DRAW */
        static Buffer genIndexBuffer(GLenum usage = GL_STATIC_DRAW);

    private:
		std::shared_ptr<GLuint> handle;
        GLenum target;
        GLenum usage;
    };
    
} // namespace cgl

#endif // CGL_BUFFER_H_