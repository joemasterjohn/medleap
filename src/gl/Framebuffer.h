#ifndef __GL_FRAMEBUFFER_H__
#define __GL_FRAMEBUFFER_H__

#include "gl/glew.h"
#include "gl/Texture.h"
#include "gl/Renderbuffer.h"
#include <memory>

namespace gl
{
    /** Pointer to an OpenGL framebuffer object */
    class Framebuffer
    {
    public:
        /** Creates a framebuffer object and allocates OpenGL resources */
        Framebuffer();
        
		/** Returns the handle to the OpenGL resource, or 0 if none. */
		GLuint id() const;

		/** Allocates an OpenGL resource and makes this object point to it */
		void generate(GLenum target = GL_FRAMEBUFFER);
        
		/** Clears this pointer. If no other objects point to the OpenGL resource, it will be destroyed. */
		void release();

        /** Binds the framebuffer to its current target */
        void bind() const;
        
        /** Unbinds any framebuffer from this framebuffer's current target */
        void unbind() const;
        
        /** Sets the target to which the framebuffer is bound. */
        void target(GLenum target);
        
        /** Sets a texture's MIP level as a color target at attachment i */
        void colorTarget(int i, const Texture& texture, int level = 0);
        
        /** Sets a texture's MIP level as a depth target */
        void depthTarget(const Texture& texture, int level = 0);

		/** Sets a renderbuffer as the depth target */
		void depthTarget(const Renderbuffer& rbo);
        
    private:
		std::shared_ptr<GLuint> handle_;
        GLenum target_;
    };
    
}

#endif // __GL_FRAMEBUFFER_H__