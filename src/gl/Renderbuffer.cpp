#include "Renderbuffer.h"

using namespace gl;

Renderbuffer::Renderbuffer() : handle(nullptr)
{
}

GLuint Renderbuffer::id() const
{
	return handle ? *(handle.get()) : 0;
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
	handle = std::shared_ptr<GLuint>(p, deleteFunction);
}

void Renderbuffer::release()
{
	handle = nullptr;
}

void Renderbuffer::bind() const
{
	if (handle)
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