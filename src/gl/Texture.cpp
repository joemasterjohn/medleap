#include "Texture.h"

using namespace gl;

Texture::Texture() : handle_(nullptr), target_(GL_INVALID_ENUM), width_(0), height_(0), depth_(0)
{
}

GLuint Texture::id() const
{
	return handle_ ? *(handle_.get()) : 0;
}

void Texture::generate(GLenum target)
{
	this->target_ = target;

	auto deleteFunction = [=](GLuint* p) {
		if (p) {
			glDeleteTextures(1, p);
			delete p;
		}
	};

	GLuint* p = new GLuint;
	glGenTextures(1, p);
	handle_ = std::shared_ptr<GLuint>(p, deleteFunction);
}

void Texture::release()
{
	handle_ = nullptr;
}

GLuint Texture::width() const
{
    return width_;
}

GLuint Texture::height() const
{
    return height_;
}

GLuint Texture::depth() const
{
    return depth_;
}

GLenum Texture::target() const
{
	return target_;
}

void Texture::bind() const
{
	if (handle_)
		glBindTexture(target_, id());
}

void Texture::unbind() const
{
	glBindTexture(target_, 0);
}

void Texture::setData1D(GLint level,
                        GLint internalFormat,
                        GLsizei width,
                        GLenum format,
                        GLenum type,
                        const GLvoid *data)
{
    this->width_ = width;
    this->height_ = 1;
    this->depth_ = 1;
	glTexImage1D(target_, level, internalFormat, width, 0, format, type, data);
}

void Texture::setData2D(GLint level,
                        GLint internalFormat,
                        GLsizei width,
                        GLsizei height,
                        GLenum format,
                        GLenum type,
                        const GLvoid *data)
{
    this->width_ = width;
    this->height_ = height;
    this->depth_ = 1;
	glTexImage2D(target_, level, internalFormat, width, height, 0, format, type, data);
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
	this->width_ = width;
	this->height_ = height;
	this->depth_ = depth;
	glTexImage3D(target_, level, internalFormat, width, height, depth, 0, format, type, data);
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
	glTexParameteri(target_, pname, param);
}

void Texture::setParameter(GLenum pname, GLfloat param)
{
	glTexParameterf(target_, pname, param);
}
