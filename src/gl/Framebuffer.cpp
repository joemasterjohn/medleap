#include "Framebuffer.h"

using namespace cgl;

Framebuffer::Framebuffer()
{
    target = GL_FRAMEBUFFER;
    glGenFramebuffers(1, &id);
}

Framebuffer::Framebuffer(GLenum target)
{
    this->target = target;
    glGenFramebuffers(1, &id);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &id);
}

void Framebuffer::bind()
{
    glBindFramebuffer(target, id);
}

void Framebuffer::unbind()
{
    glBindFramebuffer(target, 0);
}

void Framebuffer::setTarget(GLenum target)
{
    this->target = target;
}

void Framebuffer::setColorTarget(int i, cgl::Texture* texture, int level)
{
    glFramebufferTexture2D(target, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->getID(), level);
}

void Framebuffer::setDepthTarget(cgl::Texture* texture, int level)
{
    glFramebufferTexture2D(target, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->getID(), level);
}