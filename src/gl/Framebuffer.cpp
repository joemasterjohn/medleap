#include "Framebuffer.h"

using namespace gl;

Framebuffer::Framebuffer() : handle(nullptr), target(GL_INVALID_ENUM)
{
}

GLuint Framebuffer::id() const
{
	return handle ? *(handle.get()) : 0;
}

void Framebuffer::generate(GLenum target)
{
	this->target = target;

	auto deleteFunction = [=](GLuint* p) {
		if (p) {
			glDeleteFramebuffers(1, p);
			delete p;
		}
	};

	GLuint* p = new GLuint;
	glGenFramebuffers(1, p);
	handle = std::shared_ptr<GLuint>(p, deleteFunction);
}

void Framebuffer::release()
{
	handle = nullptr;
}

void Framebuffer::bind() const
{
	if (handle)
		glBindFramebuffer(target, id());
}

void Framebuffer::unbind() const
{
    glBindFramebuffer(target, 0);
}

void Framebuffer::setTarget(GLenum target)
{
    this->target = target;
}

void Framebuffer::setColorTarget(int i, const Texture& texture, int level)
{
    glFramebufferTexture2D(target,
                           GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D,
						   texture.id(),
                           level);
}

void Framebuffer::setDepthTarget(const Texture& texture, int level)
{
    glFramebufferTexture2D(target,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           texture.id(),
                           level);
}

void Framebuffer::setDepthTarget(const Renderbuffer& rbo)
{
	glFramebufferRenderbuffer(target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo.id());
}