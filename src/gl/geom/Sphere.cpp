#include "Sphere.h"
#include "Box.h"

using namespace gl;

Sphere::Sphere(Vec3 center, float radius) : center_(center), radius_(radius)
{
}

Geometry Sphere::triangles(unsigned segments) const
{
	Geometry g = Box({ 0.0f, 0.0f, 0.0f }, 1.0f).triangles(segments, segments, segments);

	for (Vec3& v : g.vertices) {
		v = v.normal() * radius_ + center_;
	}

	return g;
}