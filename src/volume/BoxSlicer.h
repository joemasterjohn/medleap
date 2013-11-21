#ifndef BOXSLICER_H
#define BOXSLICER_H

#include <vector>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "util/Camera.h"
#include "volume/BoundingBox.h"
#include <GL/glew.h>

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
    void slice(const BoundingBox& bounds, const cgl::Camera& camera, int slices);
    
    /** Vertex positions */
    const std::vector<cgl::Vec3>& getVertices();
    
    /** Indices for the geometry */
    const std::vector<GLushort>& getIndices();
    
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
    
    cgl::Vec3 up;
    cgl::Vec3 right;
    cgl::Vec3 normal;
    std::vector<cgl::Vec3> vertices;
    std::vector<GLushort> indices;
    const GLushort primRestartIndex;
    
    void slicePlane(const cgl::Vec3& p, const BoundingBox& bounds);
};

#endif // BOXSLICER_H
