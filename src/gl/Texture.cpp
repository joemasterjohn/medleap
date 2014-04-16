#include "Texture.h"

using namespace gl;

Texture::Texture() : handle(nullptr), target(GL_INVALID_ENUM), width(0), height(0), depth(0)
{
}

GLuint Texture::id() const
{
	return handle ? *(handle.get()) : 0;
}

void Texture::generate(GLenum target)
{
	this->target = target;

	auto deleteFunction = [=](GLuint* p) {
		if (p) {
			glDeleteTextures(1, p);
			delete p;
		}
	};

	GLuint* p = new GLuint;
	glGenTextures(1, p);
	handle = std::shared_ptr<GLuint>(p, deleteFunction);
}

void Texture::release()
{
	handle = nullptr;
}

GLuint Texture::getWidth() const
{
    return width;
}

GLuint Texture::getHeight() const
{
    return height;
}

GLuint Texture::getDepth() const
{
    return depth;
}

GLenum Texture::getTarget() const
{
    return target;
}

void Texture::bind() const
{
	if (handle)
		glBindTexture(target, id());
}

void Texture::unbind() const
{
    glBindTexture(target, 0);
}

void Texture::setData1D(GLint level,
                        GLint internalFormat,
                        GLsizei width,
                        GLenum format,
                        GLenum type,
                        const GLvoid *data)
{
    this->width = width;
    this->height = 1;
    this->depth = 1;
    glTexImage1D(target, level, internalFormat, width, 0, format, type, data);
}

void Texture::setData2D(GLint level,
                        GLint internalFormat,
                        GLsizei width,
                        GLsizei height,
                        GLenum format,
                        GLenum type,
                        const GLvoid *data)
{
    this->width = width;
    this->height = height;
    this->depth = 1;
    glTexImage2D(target, level, internalFormat, width, height, 0, format, type, data);
}

void Texture::setData2D(GLint internalFormat,
                        GLsizei width,
                        GLsizei height,
                        GLenum format,
                        GLenum type,
                        const GLvoid *data)
{
    setData2D(0, internalFormat, width, height, format, type, data);
}

void Texture::setData3D(GLint level,
				GLint internalFormat,
			    GLsizei width,
			    GLsizei height,
			    GLsizei depth,
			    GLenum format,
			    GLenum type,
			    const GLvoid* data)
{
	glTexImage3D(target, level, internalFormat, width, height, depth, 0, format, type, data);
}

void Texture::setData3D(GLint internalFormat,
			    GLsizei width,
			    GLsizei height,
			    GLsizei depth,
			    GLenum format,
			    GLenum type,
			    const GLvoid* data)
{
	setData3D(0, internalFormat, width, height, depth, format, type, data);
}

void Texture::setParameter(GLenum pname, GLint param)
{
    glTexParameteri(target, pname, param);
}

void Texture::setParameter(GLenum pname, GLfloat param)
{
    glTexParameterf(target, pname, param);
}
