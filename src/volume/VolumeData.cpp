#include "VolumeData.h"

VolumeData::VolumeData(unsigned char* data, int width, int height, int depth, GLenum format, GLenum type) :
    width(width), height(height), depth(depth), data(data), type(type), format(format)
{}

unsigned char* VolumeData::imagePointer(int depth)
{
    // 2 only because I'm hard-coding UINT16 right now
    return data + width * height * depth * 2;
}

void VolumeData::loadTexture2D(GLuint &texture, int depth)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // basic filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, format, type, imagePointer(depth));
}