#include "VolumeData.h"
#include "util/Util.h"

using namespace gl;
using namespace std;

VolumeData::VolumeData()
{
    width = 0;
    height = 0;
    depth = 0;
    format = 0;
    type = 0;
    minVoxelValue = 0;
    maxVoxelValue = 0;
    voxelSize.x = voxelSize.y = voxelSize.z = 0;
    bounds = new BoundingBox(1,1,1);
    modality = UNKNOWN;
    activeWindow = 0;
}

VolumeData::~VolumeData()
{
}

void VolumeData::loadTexture2D(GLuint &texture, int depth)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat(), width, height, 0, format, type, data + depth * getImageSize());
}

void VolumeData::loadTexture3D(Texture* texture)
{
    texture->bind();
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat(), width, height, depth, 0, format, type, data);
}

void VolumeData::loadGradientTexture(Texture* texture)
{
    // gradient texture will be 8-bits per channel
    unsigned char* data = new unsigned char[width * height * depth * 3];
    
    Vec3 rangeGradient = maxGradient - minGradient;
    
    unsigned char* p = data;
    for (Vec3 g : gradients) {
        *p++ = ((g.x - minGradient.x) / rangeGradient.x) * 255;
        *p++ = ((g.y - minGradient.y) / rangeGradient.y) * 255;
        *p++ = ((g.z - minGradient.z) / rangeGradient.z) * 255;
    }
    
    texture->bind();
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    delete[] data;
}

void VolumeData::setVoxelSize(float x, float y, float z)
{
    this->voxelSize.x = x;
    this->voxelSize.y = y;
    this->voxelSize.z = z;
    
    Vec3 v = getDimensionsMM();
    v.normalize();
    
    delete bounds;
    bounds = new BoundingBox(v.x, v.y, v.z);
}

const BoundingBox& VolumeData::getBounds()
{
    return *bounds;
}

const Vec3& VolumeData::getVoxelSize() const
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

unsigned int VolumeData::getNumVoxels()
{
    return width * height * depth;
}

int VolumeData::getMinValue()
{
    return minVoxelValue;
}

int VolumeData::getMaxValue()
{
    return maxVoxelValue;
}

GLenum VolumeData::getType()
{
    return type;
}

char* VolumeData::getData()
{
    return data;
}

const Vec3& VolumeData::getMinGradient()
{
    return minGradient;
}

const Vec3& VolumeData::getMaxGradient()
{
    return maxGradient;
}

Vec3 VolumeData::getDimensionsMM() const
{
    return Vec3(width * voxelSize.x,
                height * voxelSize.y,
                depth * voxelSize.z);
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

VolumeData::Modality VolumeData::getModality()
{
    return modality;
}

const Mat3& VolumeData::getPatientBasis() const
{
    return orientation;
}

vector<Window>& VolumeData::getWindows()
{
    return windows;
}

bool VolumeData::hasValue(uint16_t G, uint16_t E)
{
    return reader.GetFile().GetDataSet().FindDataElement(gdcm::Tag(G,E));
}

Window& VolumeData::getCurrentWindow()
{
    return windows[activeWindow];
}

void VolumeData::setNextWindow()
{
    activeWindow = (activeWindow + 1) % windows.size();
}

void VolumeData::setPrevWindow()
{
    if (activeWindow == 0)
        activeWindow = windows.size() - 1;
    else
        activeWindow = (activeWindow - 1) % windows.size();
}




