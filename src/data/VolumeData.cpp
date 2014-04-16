#include "VolumeData.h"
#include "util/Util.h"

using namespace gl;
using namespace std;

VolumeData::VolumeData()
{
	data = NULL;
	name = "Unknown";
    width = 0;
    height = 0;
    depth = 0;
    format = 0;
    type = 0;
    minVoxelValue = 0;
    maxVoxelValue = 0;
    bounds = new BoundingBox(1,1,1);
    modality = UNKNOWN;
    activeWindow = 0;
}

VolumeData::~VolumeData()
{
	if (data) delete[] data;
	if (bounds) delete bounds;
}

void VolumeData::setVoxelSize(float x, float y, float z)
{
    this->voxelSize.x = x;
    this->voxelSize.y = y;
    this->voxelSize.z = z;
    
    Vec3 v = getSizeMillimeters();
    v.normalize();
    
    delete bounds;
    bounds = new BoundingBox(v.x, v.y, v.z);
}


const std::string& VolumeData::getName() const
{
	return name;
}

const BoundingBox& VolumeData::getBounds() const
{
    return *bounds;
}

Vec3 VolumeData::getVoxelSizeMillimeters() const
{
	return voxelSize;
}

Vector3<unsigned> VolumeData::getSizeVoxels() const
{
	return Vector3<unsigned>(width, height, depth);
}

Vec3 VolumeData::getSizeMillimeters() const
{
	return Vec3(
		width * voxelSize.x,
		height * voxelSize.y,
		depth * voxelSize.z);
}

size_t VolumeData::getSizeBytes() const
{
	return getSliceSizeBytes() * depth;
}

size_t VolumeData::getSliceSizeBytes() const
{
	return getPixelSizeBytes() * width * height;
}

size_t VolumeData::getPixelSizeBytes() const
{
	return gl::sizeOf(type);
}

unsigned int VolumeData::getWidth() const
{
    return width;
}

unsigned int VolumeData::getHeight() const
{
    return height;
}

unsigned int VolumeData::getDepth() const
{
    return depth;
}

unsigned int VolumeData::getNumVoxels() const
{
    return width * height * depth;
}

GLenum VolumeData::getType() const
{
	return type;
}

GLenum VolumeData::getFormat() const
{
	return format;
}

bool VolumeData::isSigned() const
{
	return type == GL_BYTE || type == GL_SHORT;
}

int VolumeData::getMinValue() const
{
    return minVoxelValue;
}

int VolumeData::getMaxValue() const
{
    return maxVoxelValue;
}

VolumeData::Modality VolumeData::getModality() const
{
	return modality;
}

const Mat3& VolumeData::getPatientBasis() const
{
	return orientation;
}

const std::vector<Vec3>& VolumeData::getGradients() const
{
	return gradients;
}

Vec3 VolumeData::getMinGradient() const
{
	return minGradient;
}

Vec3 VolumeData::getMaxGradient() const
{
	return maxGradient;
}

vector<Window>& VolumeData::getWindows()
{
	return windows;
}

Window& VolumeData::getCurrentWindow()
{
	return windows[activeWindow];
}

char* VolumeData::getData()
{
    return data;
}

void VolumeData::setNextWindow()
{
    activeWindow = (activeWindow + 1) % windows.size();
}

void VolumeData::setPrevWindow()
{
    if (activeWindow == 0)
        activeWindow = static_cast<unsigned>(windows.size() - 1);
    else
		activeWindow = static_cast<unsigned>((activeWindow - 1) % windows.size());
}




