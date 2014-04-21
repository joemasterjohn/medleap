#include "Framebuffer.h"

using namespace gl;

Framebuffer::Framebuffer() : handle_(nullptr), target_(GL_INVALID_ENUM)
{
}

GLuint Framebuffer::id() const
{
	return handle_ ? *(handle_.get()) : 0;
}

void Framebuffer::generate(GLenum target)
{
	this->target_ = target;

	auto deleteFunction = [=](GLuint* p) {
		if (p) {
			glDeleteFramebuffers(1, p);
			delete p;
		}
	};

	GLuint* p = new GLuint;
	glGenFramebuffers(1, p);
	handle_ = std::shared_ptr<GLuint>(p, deleteFunction);
}

void Framebuffer::release()
{
	handle_ = nullptr;
}

void Framebuffer::bind() const
{
	if (handle_)
		glBindFramebuffer(target_, id());
}

void Framebuffer::unbind() const
{
	glBindFramebuffer(target_, 0);
}

void Framebuffer::target(GLenum target)
{
	this->target_ = target;
}

void Framebuffer::colorTarget(int i, const Texture& texture, int level)
{
	glFramebufferTexture2D(target_,
                           GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D,
						   texture.id(),
                           level);
}

void Framebuffer::depthTarget(const Texture& texture, int level)
{
    glFramebufferTexture2D(target_,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           texture.id(),
                           level);
}

void Framebuffer::depthTarget(const Renderbuffer& rbo)
{
	glFramebufferRenderbuffer(target_, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo.id());
}