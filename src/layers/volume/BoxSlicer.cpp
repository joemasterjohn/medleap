#include "BoxSlicer.h"
#include <limits>
#include <algorithm>
#include "gl/math/Vector4.h"

using namespace std;
using namespace gl;

BoxSlicer::VertexSorter::VertexSorter(BoxSlicer* slicer) : slicer(slicer)
{
}

float BoxSlicer::VertexSorter::sortValue(int vertexIndex)
{
    Vec3 v = slicer->vertices[vertexIndex];
    v.normalize();
    
    float val = (slicer->up.cross(v)).dot(slicer->normal);
    if ((v.cross(slicer->right)).dot(slicer->normal) >= 0)
        val = 2 - val;
    return val;
}

bool BoxSlicer::VertexSorter::operator()(unsigned short i, unsigned short j)
{
    return sortValue(i) < sortValue(j);
}

BoxSlicer::BoxSlicer() : primRestartIndex(65535)
{
}

const vector<Vec3>& BoxSlicer::getVertices()
{
    return vertices;
}

const vector<GLushort>& BoxSlicer::getIndices()
{
    return indices;
}

GLushort BoxSlicer::getPrimRestartIndex()
{
    return primRestartIndex;
}

float BoxSlicer::samplingLength() const
{
	return sample_length_;
}

int BoxSlicer::sliceCount() const
{
	return slice_count_;
}

float BoxSlicer::samplingLength(const Box& bounds, const Camera& camera, int numSamples) const
{
	// determine the view length by projecting bounding box vertices and
	// taking difference of max and min distances of vertices from eye
	float minDistance = numeric_limits<float>::infinity();
	float maxDistance = -numeric_limits<float>::infinity();
	for (Vec3 vertex : bounds.vertices()) {
		Vec4 v = camera.getView() * Vec4(vertex.x, vertex.y, vertex.z, 1.0f);
		float distance = -v.z;
		if (distance < minDistance) minDistance = distance;
		if (distance > maxDistance) maxDistance = distance;
	}
	return (maxDistance - minDistance) / (numSamples+1);
}

void BoxSlicer::slice(const Box& bounds, const Camera& camera, float sampleLength, int maxSlices)
{
    up = camera.getUp();
    normal = camera.getForward() * -1;
    right = camera.getRight();
    
	// determine the view length by projecting bounding box vertices and
	// taking difference of max and min distances of vertices from eye
	float minDistance = numeric_limits<float>::infinity();
	float maxDistance = -numeric_limits<float>::infinity();
	for (Vec3 vertex : bounds.vertices()) {
		Vec4 v = camera.getView() * Vec4(vertex.x, vertex.y, vertex.z, 1.0f);
		float distance = -v.z;
		if (distance < minDistance) minDistance = distance;
		if (distance > maxDistance) maxDistance = distance;
	}
	float totalLength = maxDistance - minDistance;
    
    vertices.clear();
    indices.clear();

	slice_count_ = (int)std::round(totalLength / sampleLength);
	if (maxSlices > 0)
		slice_count_ = std::min(maxSlices, slice_count_);
	sample_length_ = totalLength / (slice_count_ + 1);

	// intersect planes with boundingbox to create slice polygons
	Vec3 step = camera.getForward() * sample_length_;
	Vec3 planePoint = camera.getEye() + camera.getForward() * maxDistance;
	for (int i = 0; i < slice_count_; ++i) {
        planePoint -= step;
        slicePlane(planePoint, bounds);
    }
}

void BoxSlicer::slicePlane(const Vec3& p, const Box& bounds)
{
    // number of indices appearing before this polygon
    unsigned short polyIndexOffset = static_cast<unsigned short>(indices.size());
    
    // to avoid duplicate vertices at corners, only the first vertex that
    // intersects a corner vertex i will be added (i.e. corners[i] == false)
    bool corners[] = { false, false, false, false, false, false, false, false };
    
    // find intersections of the plane with all edges
    for (Box::Edge e : bounds.edges()) {
        const Vec3& a = bounds.vertices()[e.first];
		const Vec3& b = bounds.vertices()[e.second];
        Vec3 d = b - a;
        
        // if n * d == 0, the edge lies on the plane (ignore it)
        GLfloat nDotD = normal.dot(d);
        if (nDotD != 0) {
            float t = -normal.dot(a - p) / nDotD;
            if (t >= 0.0f && t <= 1.0f) {
                if (t == 0.0f && !corners[e.first]) {
                    Vec3 v = a + d * t;
                    corners[e.first] = true;
                    indices.push_back(static_cast<unsigned short>(vertices.size()));
                    vertices.push_back(v);
                } else if (t == 1.0f && !corners[e.second]) {
                    Vec3 v = a + d * t;
                    corners[e.second] = true;
					indices.push_back(static_cast<unsigned short>(vertices.size()));
					vertices.push_back(v);
                } else {
                    Vec3 v = a + d * t;
					indices.push_back(static_cast<unsigned short>(vertices.size()));
					vertices.push_back(v);
                }
            }
        }
    }
    
    // check if any vertices were actually added
    if (polyIndexOffset != indices.size()) {

        // end the polygon by pushing the primitive restart index (for triangle fan)
        indices.push_back(primRestartIndex);
        
        // sort indices to form a simple polygon (don't include primRestartIndex)
        sort(indices.begin() + polyIndexOffset,
             indices.end() - 1,
             VertexSorter(this));
    }
}
