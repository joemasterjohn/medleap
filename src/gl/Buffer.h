#ifndef CGL_BUFFER_H_
#define CGL_BUFFER_H_

#include <GL/glew.h>

namespace cgl
{
    class Buffer
    {
    public:
        Buffer(GLenum target, GLenum usage);
        
        ~Buffer();
        
        /** Binds the buffer to its current target. */
        void bind();
        
        /** Sets the target to which the buffer is bound. Must be applied before a call to bind() */
        void setTarget(GLenum target);
        
        /** Sets the expected usage pattern. Must be applied before a call to setData() */
        void setUsage(GLenum usage);
        
        /** Uploads data to the OpenGL-managed buffer */
        void setData(const GLvoid* data, GLsizeiptr size);
        
        /** Uploads data to a subset of the OpenGL-managed buffer */
        void setSubData(const GLvoid* data, GLsizeiptr size, GLintptr offset);
        
    private:
        GLuint id;
        GLenum target;
        GLenum usage;
    };
    
} // namespace cgl

#endif // CGL_BUFFER_H_