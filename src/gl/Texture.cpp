#include "Texture.h"

using namespace cgl;

Texture::Texture(GLenum target) : id(0), target(target), width(0), height(0), depth(0)
{
    glGenTextures(1, &id);
}

Texture::~Texture()
{
    if (id)
        glDeleteTextures(1, &id);
}

GLuint Texture::getID() const
{
    return id;
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
    glBindTexture(target, id);
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

void Texture::setParameter(GLenum pname, GLint param)
{
    glTexParameteri(target, pname, param);
}

void Texture::setParameter(GLenum pname, GLfloat param)
{
    glTexParameterf(target, pname, param);
}
