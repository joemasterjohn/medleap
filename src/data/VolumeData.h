#ifndef __MEDLEAP_VOLUME_DATA__
#define __MEDLEAP_VOLUME_DATA__

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Window.h"
#include "BoundingBox.h"
#include "gl/math/Vector3.h"
#include "gl/Texture.h"
#include "gl/math/Matrix3.h"

/** Volumetric data stored in a regular grid of voxels. All voxel values are assumed to be an integer format (8 or 16 bits) either signed or unsigned. */
class VolumeData
{
public:
    
    /** The type of image data */
    enum Modality
    {
        CT,         // computed tomography
        MR,         // magnetic resonance
        UNKNOWN     // any other data
    };
    
    /** Clean up resources */
    ~VolumeData();

	/** Name of the volume data */
	const std::string& getName() const;
    
    /** Returns a bounding box of the normalized real world size of the volume. */
    const BoundingBox& getBounds() const;
    
	/** Size of each voxel in millimeters. Default is (1,1,1) for unknown modalities. */
	Vec3 getVoxelSizeMillimeters() const;

	/** Size of the volume in number of voxels */
	Vector3<unsigned> getSizeVoxels() const;

	/** Size of the volume in millimeters */
	Vec3 getSizeMillimeters() const;

	/** Size in bytes of the entire volume */
	size_t getSizeBytes() const;

	/** Size in bytes of a single slice in the volume (width * height * pixelSizeBytes) */
	size_t getSliceSizeBytes() const;

	/** Size in bytes of a single voxel */
	size_t getPixelSizeBytes() const;

    /** Number of voxels in X direction */
    unsigned int getWidth() const;
    
    /** Number of voxels in Y direction */
    unsigned int getHeight() const;
    
    /** Number of voxels in Z direction */
    unsigned int getDepth() const;
    
    /** Total number of voxels in the volume (width * height * depth) */
    unsigned int getNumVoxels() const;
    
    /** Stored pixel type (GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, etc.) */
    GLenum getType() const;
    
	/** Stored pixel format (currently always GL_RED / single channel) */
	GLenum getFormat() const;

    /** Data is stored in a signed format */
    bool isSigned() const;
    
    /** The minimum voxel value stored in the data */
    int getMinValue() const;
    
    /** The maximum voxel value stored in the data */
    int getMaxValue() const;

    /** Returns the modality of the data */
    Modality getModality() const;
    
	/** Matrix that transforms DICOM image space (+X right, +Y down) to patient space (+X = left, +Y = posterior, +Z = superior) */
	const Mat3& getPatientBasis() const;

	/** Returns all gradient vectors */
	const std::vector<Vec3>& getGradients() const;

	/** Vector storing minimum x, y, and z components of all gradient vectors */
	Vec3 getMinGradient() const;

	/** Vector storing maximum x, y, and z components of all gradient vectors */
	Vec3 getMaxGradient() const;

    /** Windows that store values of interest */
    std::vector<Window>& getWindows();
    
    /** The currently active voxel value window (determines which values are displayed) */
    Window& getCurrentWindow();
    
    /** Sets the size of each voxel in real world units */
    void setVoxelSize(float x, float y, float z);
    
    /** Sets the current window to the next available window */
    void setNextWindow();
    
    /** Sets the current window to the previous available window */
    void setPrevWindow();
    
    /** Pointer to the raw data bytes */
    char* getData();
    
private:

    char* data;
	std::string name;
    std::vector<Vec3> gradients;
    Vec3 minGradient;
    Vec3 maxGradient;
    float minGradientMag;
    float maxGradientMag;
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    GLenum format;
    GLenum type;
    int minVoxelValue;
    int maxVoxelValue;
    Vec3 voxelSize;
    BoundingBox* bounds;
    Modality modality;
    Mat3 orientation;
    std::vector<Window> windows;
    unsigned int activeWindow;

    /** Private constructor since loading is complex and done by the Loader class */
    VolumeData();
    
	/** Returns the value of a voxel (as a signed integer, not the type of the underlying data) */
	template <typename T> int value(unsigned int x, unsigned int y, unsigned int z)
	{
		x = std::min(std::max(0u, x), width - 1);
		y = std::min(std::max(0u, y), height - 1);
		z = std::min(std::max(0u, z), depth - 1);
		return (int)(((T*)(data))[z * width * height + y * width + x]);
	}

    /** Computes gradient vectors for this volume. Gradients are always stored as floats, regardless of the volume data type. */
    template<typename T> void computeGradients()
    {
        gradients.clear();
        minGradientMag = std::numeric_limits<float>::infinity();
        minGradient = Vec3(minGradientMag);
        maxGradientMag = -std::numeric_limits<float>::infinity();
        maxGradient = Vec3(maxGradientMag);

        // compute gradient vectors
		Vec3 scale = Vec3(1.0f) / voxelSize * 2.0f;
        for (int z = 0; z < depth; z++) {
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
					float gx = (value<T>(x - 1, y, z) - value<T>(x + 1, y, z)) * scale.x;
					float gy = (value<T>(x, y - 1, z) - value<T>(x, y + 1, z)) * scale.y;
					float gz = (value<T>(x, y, z - 1) - value<T>(x, y, z + 1)) * scale.z;
                    Vec3 g(gx, gy, gz);

                    float mag = g.length();
                    if (mag < minGradientMag) minGradientMag = mag;
                    if (mag > maxGradientMag) maxGradientMag = mag;
                    
                    if (g.x < minGradient.x) minGradient.x = g.x;
                    if (g.y < minGradient.y) minGradient.y = g.y;
                    if (g.z < minGradient.z) minGradient.z = g.z;
                    if (g.x > maxGradient.x) maxGradient.x = g.x;
                    if (g.y > maxGradient.y) maxGradient.y = g.y;
                    if (g.z > maxGradient.z) maxGradient.z = g.z;
                                        
                    gradients.push_back(g);
                }
            }
        }
    }
    
    friend class VolumeLoader;
};

#endif // __MEDLEAP_VOLUME_DATA__