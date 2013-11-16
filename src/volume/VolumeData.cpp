#include "VolumeData.h"
#include "gl/Util.h"

VolumeData::VolumeData(int width, int height, int depth, GLenum format, GLenum type) :
    width(width), height(height), depth(depth), format(format), type(type)
{
    data = new char[width * height * depth * gl::sizeOf(type)];
}

VolumeData::VolumeData(char* data, int width, int height, int depth, GLenum format, GLenum type) :
    data(data), width(width), height(height), depth(depth), format(format), type(type)
{}

void VolumeData::loadTexture2D(GLuint &texture, int depth)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // basic filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat(), width, height, 0, format, type, data + depth * getImageSize());
}

unsigned int VolumeData::getImageSize()
{
    return getPixelSize() * width * height;
}

unsigned int VolumeData::getPixelSize()
{
    return gl::sizeOf(type);
}

unsigned int VolumeData::getVolumeSize()
{
    return getImageSize() * depth;
}

GLenum VolumeData::internalFormat()
{
    if (format == GL_RED) {
        if (type == GL_BYTE)
            return GL_R8_SNORM;
        if (type == GL_SHORT)
            return GL_R16_SNORM;
        return GL_RED;
    }
    
    // for now I shouldn't be using multi-channel data, but
    // if I do let's use the same as data buffer format
    return format;
}

bool VolumeData::isSigned()
{
    return type == GL_BYTE || type == GL_SHORT;
}