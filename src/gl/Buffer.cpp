#include "Buffer.h"

using namespace gl;

Buffer::Buffer() : handle_(nullptr), target_(GL_INVALID_ENUM), usage_(GL_INVALID_ENUM)
{
}

GLuint Buffer::id() const
{
	return handle_ ? *(handle_.get()) : 0;
}

void Buffer::generate(GLenum target, GLenum usage)
{
	this->target_ = target;
	this->usage_ = usage;

	auto deleteFunction = [=](GLuint* p) {
		if (p) {
			glDeleteBuffers(1, p);
			delete p;
		}
	};

	GLuint* p = new GLuint;
	glGenBuffers(1, p);
	handle_ = std::shared_ptr<GLuint>(p, deleteFunction);
}

void Buffer::generateVBO(GLenum usage)
{
	generate(GL_ARRAY_BUFFER, usage);
}

void Buffer::generateIBO(GLenum usage)
{
	generate(GL_ELEMENT_ARRAY_BUFFER, usage);
}

void Buffer::release()
{
	handle_ = nullptr;
}

void Buffer::bind() const
{
	if (handle_)
		glBindBuffer(target_, id());
}

void Buffer::unbind() const
{
    glBindBuffer(target_, 0);
}

void Buffer::target(GLenum target)
{
    this->target_ = target;
}

void Buffer::usage(GLenum usage)
{
    this->usage_ = usage;
}

void Buffer::data(const GLvoid* data, GLsizeiptr size)
{
    glBufferData(target_, size, data, usage_);
}

void Buffer::subData(const GLvoid* data, GLsizeiptr size, GLintptr offset)
{
    glBufferSubData(target_, offset, size, data);
}