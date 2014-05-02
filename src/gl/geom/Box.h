#ifndef __GL_GEOM_BOX_H__
#define __GL_GEOM_BOX_H__

#include "gl/glew.h"
#include "gl/util/Geometry.h"
#include "Plane.h"

namespace gl
{
	/** Axis-aligned box */
    class Box
    {
    public:
		typedef std::pair<unsigned short, unsigned short> Edge;

		Box(float size);
		Box(float width, float height, float length);
		Box(const Vec3& center, float size);
		Box(const Vec3& center, float width, float height, float length);

		float width() const { return size_.x; }
		float height() const { return size_.y; }
		float length() const { return size_.z; }
		Vec3 center() const { return center_; }
		Vec3 min() const { return min_; }
		Vec3 max() const { return max_; }
		const std::vector<Vec3>& vertices() const { return vertices_; }
		const std::vector<Edge>& edges() const { return edges_; }

		void size(float size);
		void size(float width, float height, float length);
		void center(const Vec3& center);

		Geometry lines() const;
		Geometry triangles(unsigned x_segments, unsigned y_segments, unsigned z_segments) const;

		bool contains(const Vec3& p) const;
		Vec3 normalize(const Vec3& p) const;
		Vec3 clamp(const Vec3& p) const;
		std::vector<Vec3> intersect(const Plane& p) const;
    
	private:
		Vec3 center_;
		Vec3 size_;
		Vec3 min_;
		Vec3 max_;
		std::vector<Vec3> vertices_;
		std::vector<Edge> edges_;
	};
}

#endif // __CGL_GEOM_BOX_H__