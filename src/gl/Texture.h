#ifndef CGL_TEXTURE_H_
#define CGL_TEXTURE_H_

#include "gl/glew.h"

namespace gl
{
    class Texture
    {
    public:
        /** Creates a new texture */
        Texture(GLenum target);
        
        /** Releases the texture resources */
        ~Texture();
        
        /** Returns the OpenGL texture handle */
        GLuint getID() const;
        
        /** Returns the texture width. */
        GLuint getWidth() const;
        
        /** Returns the texture height. Has no relevance for 1D textures. */
        GLuint getHeight() const;
        
        /** Returns the texture depth. Has no relevance for 1D or 2D textures. */
        GLuint getDepth() const;
        
        /** Returns the current texture target (1D, 2D, 3D...) */
        GLenum getTarget() const;
        
        /** Binds the referenced texture object to its target. */
        void bind() const;
        
        /** Unbinds the referenced texture object from its target. */
        void unbind() const;
        
        /** Set 1D texture data (glTexImage1D) */
        void setData1D(GLint level,
                       GLint internalFormat,
                       GLsizei width,
                       GLenum format,
                       GLenum type,
                       const GLvoid* data);
        
        /** Set 2D texture data (glTexImage2D) */
        void setData2D(GLint level,
                       GLint internalFormat,
                       GLsizei width,
                       GLsizei height,
                       GLenum format,
                       GLenum type,
                       const GLvoid* data);
        
        /** Set 2D texture data (glTexImage2D) at MIP level 0 */
        void setData2D(GLint internalFormat,
                       GLsizei width,
                       GLsizei height,
                       GLenum format,
                       GLenum type,
                       const GLvoid* data);
        
        void setParameter(GLenum pname, GLint param);
        
        void setParameter(GLenum pname, GLfloat param);
        
    private:
        GLuint id;
        GLenum target;
        GLuint width;
        GLuint height;
        GLuint depth;
    };
    
} // namespace cgl

#endif // CGL_TEXTURE_H_
