#include "VolumeData.h"
#include "util/Util.h"

using namespace std;

VolumeData::VolumeData(int width, int height, int depth, GLenum format, GLenum type) :
    width(width), height(height), depth(depth), format(format), type(type)
{
    data = new char[width * height * depth * gl::sizeOf(type)];
    bounds = new BoundingBox(1,1,1);
}

VolumeData::VolumeData(char* data, int width, int height, int depth, GLenum format, GLenum type) :
    data(data), width(width), height(height), depth(depth), format(format), type(type)
{
    bounds = new BoundingBox(1,1,1);
}

void VolumeData::loadTexture2D(GLuint &texture, int depth)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat(), width, height, 0, format, type, data + depth * getImageSize());
}

void VolumeData::loadTexture3D(cgl::Texture* texture)
{
    texture->bind();
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat(), width, height, depth, 0, format, type, data);
}

void VolumeData::setVoxelSize(float x, float y, float z)
{
    this->voxelSize.x = x;
    this->voxelSize.y = y;
    this->voxelSize.z = z;
    
    float totalWidth = x * width;
    float totalHeight = y * height;
    float totalDepth = z * depth;
    
    // TODO: why is this not right for my CT data? Says 10 but should be 3?
    totalDepth = 3 * depth;
    
    cgl::Vec3 v(totalWidth, totalHeight, totalDepth);
    v.normalize();
    delete bounds;
    bounds = new BoundingBox(v.x, v.y, v.z);
}

const BoundingBox& VolumeData::getBounds()
{
    return *bounds;
}

const cgl::Vec3& VolumeData::getVoxelSize() const
{
    return voxelSize;
}

unsigned int VolumeData::getWidth()
{
    return width;
}

unsigned int VolumeData::getHeight()
{
    return height;
}

unsigned int VolumeData::getDepth()
{
    return depth;
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

GLenum VolumeData::getType()
{
    return type;
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