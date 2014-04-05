#ifndef CGL_BUFFER_H_
#define CGL_BUFFER_H_

#include "gl/glew.h"

namespace gl
{
    /** Buffer object for storing vertex and index data, among other things */
    class Buffer
    {
    public:
        /** Creates a new buffer an allocates OpenGL resources */
        Buffer(GLenum target, GLenum usage);
        
        /** Destroys the buffer and releases the OpenGL resources */
        ~Buffer();
        
        /** Binds the buffer to its current target. */
        void bind();
        
        /** Clears the binding between this buffer's target and any buffer */
        void unbind();
        
        /** Sets the target to which the buffer is bound. Must be applied before a call to bind() */
        void setTarget(GLenum target);
        
        /** Sets the expected usage pattern. Must be applied before a call to setData() */
        void setUsage(GLenum usage);
        
        /** Uploads data to the OpenGL-managed buffer */
        void setData(const GLvoid* data, GLsizeiptr size);
        
        /** Uploads data to a subset of the OpenGL-managed buffer */
        void setSubData(const GLvoid* data, GLsizeiptr size, GLintptr offset);
        
        /** Creates a vertex buffer (GL_ARRAY_BUFFER) with default usage GL_STATIC_DRAW */
        static Buffer* createVBO(GLenum usage = GL_STATIC_DRAW);
        
        /** Creates an index buffer (GL_ELEMENT_ARRAY_BUFFER) with default usage GL_STATIC_DRAW */
        static Buffer* createIBO(GLenum usage = GL_STATIC_DRAW);
        
    private:
        GLuint id;
        GLenum target;
        GLenum usage;
    };
    
} // namespace cgl

#endif // CGL_BUFFER_H_