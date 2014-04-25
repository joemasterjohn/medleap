#ifndef BOXSLICER_H
#define BOXSLICER_H

#include <vector>
#include "gl/math/Math.h"
#include "util/Camera.h"
#include "data/BoundingBox.h"
#include "gl/glew.h"

/**
 * Intersects a number of view-aligned planes with an axis-aligned bounding box.
 * The output is vertex and index data that define the intersection polygons. Each
 * polygon is represented as a triangle fan, and a primitive restart index is inserted
 * to mark the end of each polygon. Indices are unsigned ints: assuming the worst case
 * and each slice has 6 vertices, that's still over 10,000 slices. This is too many
 * slices to even bother with anyway.
 */
class BoxSlicer
{
public:
    BoxSlicer();
    
    /** Cuts the box into slices and stores the data in this class */
    void slice(const BoundingBox& bounds, const Camera& camera, float sampleLength, int maxSlices);

    /** Vertex positions */
    const std::vector<gl::Vec3>& getVertices();
    
    /** Indices for the geometry */
    const std::vector<GLushort>& getIndices();
    
	/** Determine the distance between planes given a number of samples (faster than slicing to find out) */
	float samplingLength(const BoundingBox& bounds, const Camera& camera, int numSamples) const;

	/** Distance between sampling planes (updated after slice called) */
	float samplingLength() const;

	/** Number of actual slices created */
	int sliceCount() const;

    /** Index value that marks the start of a new slice */
    GLushort getPrimRestartIndex();
    
private:
    /** For sorting vertex indices to make simple polygons. */
    class VertexSorter {
    public:
        VertexSorter(BoxSlicer* slicer);
        float sortValue(int vertexIndex);
        bool operator() (unsigned short i, unsigned short j);
    private:
        BoxSlicer* slicer;
    };
    
    friend class VertexSorter;
    
	gl::Vec3 up;
	gl::Vec3 right;
	gl::Vec3 normal;
	std::vector<gl::Vec3> vertices;
    std::vector<GLushort> indices;
    const GLushort primRestartIndex;
	float sample_length_;
	int slice_count_;
    
	void slicePlane(const gl::Vec3& p, const BoundingBox& bounds);
};

#endif // BOXSLICER_H
