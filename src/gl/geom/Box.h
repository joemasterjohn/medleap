#ifndef __GL_GEOM_BOX_H__
#define __GL_GEOM_BOX_H__

#include "gl/glew.h"
#include "gl/util/Geometry.h"
#include "Plane.h"

namespace gl
{
    class Box
    {
    public:
		typedef std::pair<unsigned short, unsigned short> Edge;

		Box(float size);
		Box(float width, float height, float length);

		float width() const { return size_.x; }
		float height() const { return size_.y; }
		float length() const { return size_.z; }
		const std::vector<Vec3>& vertices() const { return vertices_; }
		const std::vector<Edge>& edges() const { return edges_; }
		Vec3 min() const;
		Vec3 max() const;

		Geometry lines() const;
		Geometry triangles(unsigned x_segments, unsigned y_segments, unsigned z_segments) const;

		std::vector<Vec3> intersect(const Plane& p) const;
    
	private:
		Vec3 size_;
		std::vector<Vec3> vertices_;
		std::vector<Edge> edges_;
	};
}

#endif // __CGL_GEOM_BOX_H__