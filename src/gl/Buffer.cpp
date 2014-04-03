#include "Buffer.h"

using namespace cgl;

Buffer::Buffer(GLenum target, GLenum usage) : target(target), usage(usage)
{
    glGenBuffers(1, &id);
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &id);
}

void Buffer::bind()
{
    glBindBuffer(target, id);
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