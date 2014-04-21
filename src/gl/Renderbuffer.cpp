#include "Renderbuffer.h"

using namespace gl;

Renderbuffer::Renderbuffer() : handle_(nullptr)
{
}

GLuint Renderbuffer::id() const
{
	return handle_ ? *(handle_.get()) : 0;
}

void Renderbuffer::generate()
{
	auto deleteFunction = [=](GLuint* p) {
		if (p) {
			glDeleteRenderbuffers(1, p);
			delete p;
		}
	};

	GLuint* p = new GLuint;
	glGenRenderbuffers(1, p);
	handle_ = std::shared_ptr<GLuint>(p, deleteFunction);
}

void Renderbuffer::release()
{
	handle_ = nullptr;
}

void Renderbuffer::bind() const
{
	if (handle_)
		glBindRenderbuffer(GL_RENDERBUFFER, id());
}

void Renderbuffer::unbind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderbuffer::storage(GLenum internalFormat, GLsizei width, GLsizei height)
{
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
}

void Renderbuffer::storage(GLenum internalFormat, GLsizei width, GLsizei height, GLsizei samples)
{
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, width, height);
}