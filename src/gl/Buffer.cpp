#include "Buffer.h"

using namespace gl;

Buffer::Buffer() : handle(nullptr), target(GL_INVALID_ENUM), usage(GL_INVALID_ENUM)
{
}

GLuint Buffer::id() const
{
	return handle ? *(handle.get()) : 0;
}

void Buffer::generate(GLenum target, GLenum usage)
{
	this->target = target;
	this->usage = usage;

	auto deleteFunction = [=](GLuint* p) {
		if (p) {
			glDeleteBuffers(1, p);
			delete p;
		}
	};

	GLuint* p = new GLuint;
	glGenBuffers(1, p);
	handle = std::shared_ptr<GLuint>(p, deleteFunction);
}

void Buffer::release()
{
	handle = nullptr;
}

void Buffer::bind() const
{
	if (handle)
		glBindBuffer(target, id());
}

void Buffer::unbind() const
{
    glBindBuffer(target, 0);
}

void Buffer::setTarget(GLenum target)
{
    this->target = target;
}

void Buffer::setUsage(GLenum usage)
{
    this->usage = usage;
}

void Buffer::setData(const GLvoid* data, GLsizeiptr size)
{
    glBufferData(target, size, data, usage);
}

void Buffer::setSubData(const GLvoid* data, GLsizeiptr size, GLintptr offset)
{
    glBufferSubData(target, offset, size, data);
}

Buffer Buffer::genVertexBuffer(GLenum usage)
{
	Buffer buf;
	buf.generate(GL_ARRAY_BUFFER, usage);
	return buf;
}

Buffer Buffer::genIndexBuffer(GLenum usage)
{
	Buffer buf;
	buf.generate(GL_ELEMENT_ARRAY_BUFFER, usage);
	return buf;
}