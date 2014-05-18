#ifndef __CGL_GEOM_PLANE_H__
#define __CGL_GEOM_PLANE_H__

#include "gl/glew.h"
#include "gl/util/Geometry.h"

namespace gl
{
    class Plane
    {
	public:
		/** Creates a plane with unit-length normal and distance from origin */
		Plane(const Vec3& normal, float dist_from_origin);

		/** Create a plane with unit-length normal and a point on its surface */
		Plane(const Vec3& normal, const Vec3& point);

		/** Point on plane nearest to origin */
		Vec3 nearestToOrigin() const;

		/** Normal vector unit length */
		Vec3 normal() const;
		
		/** Sets normal vector */
		void normal(const Vec3& normal);

		/** Returns distance from origin */
		float distFromOrigin() const;

		/** Sets distance from origin */
		void distFromOrigin(float dist);

		/** Sets point on the plane */
		void point(const Vec3& point);

		/** Creates a triangulated surface centered at the point nearest to the plane's surface. */
		Geometry triangles(unsigned u_segments, unsigned v_segments);


	private:
		Vec3 normal_;
		float dist_origin_;
	};
}

#endif // __CGL_GEOM_PLANE_H__