#ifndef __MEDLEAP_VOLUME_DATA__
#define __MEDLEAP_VOLUME_DATA__

#include "gl/glew.h"
#include <vector>
#include <string>
#include <thread>
#include "Window.h"
#include "BoundingBox.h"
#include "gl/math/Math.h"
#include "gl/Texture.h"

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
	gl::Vec3 getVoxelSizeMillimeters() const;

	/** Size of the volume in number of voxels */
	gl::Vector3<unsigned> getSizeVoxels() const;

	/** Size of the volume in millimeters */
	gl::Vec3 getSizeMillimeters() const;

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
	const gl::Mat3& getPatientBasis() const;

	/** Returns all gradient vectors */
	const std::vector<gl::Vec3>& getGradients() const;

	/** Vector storing minimum x, y, and z components of all gradient vectors */
	gl::Vec3 getMinGradient() const;

	/** Vector storing maximum x, y, and z components of all gradient vectors */
	gl::Vec3 getMaxGradient() const;

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
	std::vector<gl::Vec3> gradients;
	gl::Vec3 minGradient;
	gl::Vec3 maxGradient;
    float minGradientMag;
    float maxGradientMag;
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    GLenum format;
    GLenum type;
    int minVoxelValue;
    int maxVoxelValue;
	gl::Vec3 voxelSize;
    BoundingBox* bounds;
    Modality modality;
	gl::Mat3 orientation;
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
		using namespace gl;

        gradients.clear();
		gradients.resize(width * height * depth);

		const int numThreads = 4;

		float threadMinMag[numThreads];
		float threadMaxMag[numThreads];
		Vec3 threadMin[numThreads];
		Vec3 threadMax[numThreads];
		for (int i = 0; i < numThreads; ++i) {
			threadMinMag[i] = +std::numeric_limits<float>::infinity();
			threadMaxMag[i] = -std::numeric_limits<float>::infinity();
			threadMin[i] = Vec3(+std::numeric_limits<float>::infinity());
			threadMax[i] = Vec3(-std::numeric_limits<float>::infinity());
		}

		Vec3 scale = Vec3(1.0f) / voxelSize * 2.0f;

		auto work = [&](int threadIndex, int start, int end) {
			for (int z = start; z < end; z++) {
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						float gx = (value<T>(x - 1, y, z) - value<T>(x + 1, y, z)) * scale.x;
						float gy = (value<T>(x, y - 1, z) - value<T>(x, y + 1, z)) * scale.y;
						float gz = (value<T>(x, y, z - 1) - value<T>(x, y, z + 1)) * scale.z;
						Vec3 g(gx, gy, gz);
						float mag = g.length();

						if (mag < threadMinMag[threadIndex]) threadMinMag[threadIndex] = mag;
						if (mag > threadMaxMag[threadIndex]) threadMaxMag[threadIndex] = mag;
						if (g.x < threadMin[threadIndex].x) threadMin[threadIndex].x = g.x;
						if (g.y < threadMin[threadIndex].y) threadMin[threadIndex].y = g.y;
						if (g.z < threadMin[threadIndex].z) threadMin[threadIndex].z = g.z;
						if (g.x > threadMax[threadIndex].x) threadMax[threadIndex].x = g.x;
						if (g.y > threadMax[threadIndex].y) threadMax[threadIndex].y = g.y;
						if (g.z > threadMax[threadIndex].z) threadMax[threadIndex].z = g.z;

						gradients[z * width * height + y * width + x] = g;
					}
				}
			}
		};

		std::vector<std::thread> threads;
		int slicesPerThread = depth / numThreads;
		int remainder = depth % numThreads;

		int start = 0;
		int end = slicesPerThread;
		for (int i = 0; i < numThreads; i++) {
			if (i < remainder)
				end++;
			threads.push_back(std::move(thread(work, i, start, end)));
			start = end;
			end += slicesPerThread;
		}

		auto vecMin = [](Vec3&a, Vec3& b)->Vec3 {
			return Vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
		};

		auto vecMax = [](Vec3&a, Vec3& b)->Vec3 {
			return Vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
		};


		minGradientMag = std::numeric_limits<float>::infinity();
		minGradient = Vec3(minGradientMag);
		maxGradientMag = -std::numeric_limits<float>::infinity();
		maxGradient = Vec3(maxGradientMag);
		for (int i = 0; i < numThreads; ++i) {
			threads[i].join();
			minGradient = vecMin(minGradient, threadMin[i]);
			maxGradient = vecMax(maxGradient, threadMax[i]);
			minGradientMag = std::min(minGradientMag, threadMinMag[i]);
			maxGradientMag = std::min(maxGradientMag, threadMaxMag[i]);
		}
    }
    
    friend class VolumeLoader;
};

#endif // __MEDLEAP_VOLUME_DATA__