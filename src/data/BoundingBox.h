#ifndef __medleap__BoundingBox__
#define __medleap__BoundingBox__

#include <vector>
#include <utility>
#include "gl/math/Math.h"

/** Contains vertex positions and edges (indices) for an axis-aligned bounding box */
class BoundingBox
{
public:
    typedef std::pair<unsigned short, unsigned short> Edge;
    
    BoundingBox(float width, float height, float depth);
    
    float getWidth() const;
    float getHeight() const;
    float getLength() const;
    gl::Vec3 getMinimum() const;
    gl::Vec3 getMaximum() const;
	const std::vector<gl::Vec4>& getVertices() const;
    const std::vector<Edge>& getEdges() const;
    
private:
	gl::Vec3 size;
	std::vector<gl::Vec4> vertices;
    std::vector<Edge> edges;
};

#endif /* defined(__medleap__BoundingBox__) */
