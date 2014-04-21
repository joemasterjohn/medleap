#ifndef __GL_TEXTURE_H__
#define __GL_TEXTURE_H__

#include "gl/glew.h"
#include <memory>

namespace gl
{
    class Texture
    {
    public:
        /** Creates an empty texture pointer */
        Texture();
        
		/** Returns the handle to the OpenGL resource, or 0 if none. */
		GLuint id() const;

		/** Creates a new OpenGL resource. This object will point to it. */
		void generate(GLenum target);

		/** Clears this pointer. If no other objects point to the OpenGL resource, it will be destroyed. */
		void release();
        
        /** Returns the texture width. */
        GLuint width() const;
        
        /** Returns the texture height. Has no relevance for 1D textures. */
        GLuint height() const;
        
        /** Returns the texture depth. Has no relevance for 1D or 2D textures. */
        GLuint depth() const;
        
        /** Returns the current texture target (1D, 2D, 3D...) */
        GLenum target() const;
        
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

		/** Set 1D texture data (glTexImage1D) at MIP level 0 */
		void setData1D(GLint internalFormat,
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

		/** Set 3D texture data (glTexImage3D) */
		void setData3D(GLint level,
					   GLint internalFormat,
			           GLsizei width,
			           GLsizei height,
			           GLsizei depth,
			           GLenum format,
			           GLenum type,
			           const GLvoid* data);

		/** Set 3D texture data (glTexImage3D) at MIP level 0 */
		void setData3D(GLint internalFormat,
			           GLsizei width,
			           GLsizei height,
			           GLsizei depth,
			           GLenum format,
			           GLenum type,
			           const GLvoid* data);

        
        void setParameter(GLenum pname, GLint param);
        
        void setParameter(GLenum pname, GLfloat param);
        
    private:
		std::shared_ptr<GLuint> handle_;
        GLenum target_;
        GLuint width_;
        GLuint height_;
        GLuint depth_;
    };
    
}

#endif // __GL_TEXTURE_H__
