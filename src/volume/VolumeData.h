#ifndef __MEDLEAP_VOLUME_DATA__
#define __MEDLEAP_VOLUME_DATA__

#include <GL/glew.h>
#include <vector>
#include "Window.h"
#include "volume/BoundingBox.h"
#include "math/Vector3.h"
#include "gl/Texture.h"

/** Base class for any volumetric data that is stored in a regular grid of voxels. **/
class VolumeData
{
public:
    /** Creates an empty volume with memory allocated to fit the specified dimensions */
    VolumeData(int width, int height, int depth, GLenum format, GLenum type);
    
    /** Creates a volume using the memory stored in data; does not copy or allocate new memory */
    VolumeData(char* data, int width, int height, int depth, GLenum format, GLenum type);
    
    /** Dimensions of a voxel in real world units */
    const cgl::Vec3& getVoxelSize() const;
    
    /** Sets the size of each voxel in real world units */
    void setVoxelSize(float x, float y, float z);
    
    /** Stores the image at depth into the provided 2D texture */
    void loadTexture2D(GLuint& texture, int depth);
    
    /** Stores all images into a 3D texture */
    void loadTexture3D(cgl::Texture* texture);
    
    /** Returns a bounding box of the normalized real world dimensions of the volume */
    const BoundingBox& getBounds();
    
    /** Number of pixels horizontally in each slice */
    unsigned int getWidth();
    
    /** Number of pixels vertically in each slice */
    unsigned int getHeight();
    
    /** Number of slices */
    unsigned int getDepth();
    
    /** Size in bytes of a single slice/image */
    unsigned int getImageSize();
    
    /** Size in bytes of a single pixel */
    unsigned int getPixelSize();
    
    /** Size in bytes of all slices */
    unsigned int getVolumeSize();
    
    /** Type for channels in the images */
    GLenum getType();
    
    /** Data is stored in a signed format */
    bool isSigned();

protected:
    char* data;
    int width;
    int height;
    int depth;
    GLenum format;
    GLenum type;
    cgl::Vec3 voxelSize;
    BoundingBox* bounds;
    
    GLenum internalFormat();
};

#endif // __MEDLEAP_VOLUME_DATA__