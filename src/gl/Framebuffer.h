#ifndef CGL_FRAMEBUFFER_H_
#define CGL_FRAMEBUFFER_H_

#include <GL/glew.h>
#include "gl/Texture.h"

namespace cgl
{
    class Framebuffer
    {
    public:
        Framebuffer();
        
        Framebuffer(GLenum target);
        
        ~Framebuffer();
        
        /** Binds the framebuffer to its current target */
        void bind();
        
        /** Unbinds any framebuffer from this framebuffer's current target */
        void unbind();
        
        /** Sets the target to which the framebuffer is bound. */
        void setTarget(GLenum target);
        
        /** Sets a texture's MIP level as a color target at attachment i */
        void setColorTarget(int i, cgl::Texture* texture, int level = 0);
        
        /** Sets a texture's MIP level as a depth target */
        void setDepthTarget(cgl::Texture* texture, int level = 0);
        
        
    private:
        GLuint id;
        GLenum target;
    };
    
} // namespace cgl

#endif // CGL_FRAMEBUFFER_H_