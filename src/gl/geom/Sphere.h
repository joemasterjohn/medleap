#ifndef __GL_GEOM_SPHERE_H__
#define __GL_GEOM_SPHERE_H__

#include "gl/glew.h"
#include "gl/util/Geometry.h"

namespace gl
{
	class Sphere
    {
    public:
		Sphere(Vec3 center, float radius);
		Geometry triangles(unsigned segments) const;

		void center(const Vec3& center) { center_ = center; }
		Vec3 center() const { return center_; }

	private:
		Vec3 center_;
		float radius_;
    };
}

#endif // __GL_GEOM_SPHERE_H__