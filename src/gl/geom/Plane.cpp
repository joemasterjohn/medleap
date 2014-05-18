#include "Plane.h"

using namespace gl;

Plane::Plane(const Vec3& normal, float dist_from_origin) :
	normal_(normal),
	dist_origin_(dist_from_origin)
{
}

Plane::Plane(const Vec3& normal, const Vec3& point) :
	normal_(normal),
	dist_origin_(normal.dot(point))
{
}

Vec3 Plane::nearestToOrigin() const
{
	return normal_ * dist_origin_;
}

Vec3 Plane::normal() const
{
	return normal_;
}

void Plane::normal(const Vec3& normal)
{
	normal_ = normal;
}

float Plane::distFromOrigin() const
{
	return dist_origin_;
}

void Plane::distFromOrigin(float dist)
{
	dist_origin_ = dist;
}

void Plane::point(const Vec3& point)
{
	dist_origin_ = normal_.dot(point);
}

Geometry Plane::triangles(unsigned u_segments, unsigned v_segments)
{
	Vec3 u, v;
	if (std::abs(normal_.dot(Vec3::yAxis())) > 0.999f) {
		u = { 1.0f, 0.0f, 0.0f };
		v = { 0.0f, 0.0f, -1.0f };
	} else {
		u = Vec3::yAxis().cross(normal_).normalize();
		v = normal_.cross(u).normalize();
	}

	Vec3 o = nearestToOrigin();

	Geometry g;
	g.mode = GL_TRIANGLES;

	unsigned i = 0;
	for (unsigned iv = 0; iv <= v_segments; ++iv) {
		float nv = static_cast<float>(iv) / v_segments - 0.5f;

		for (unsigned iu = 0; iu <= u_segments; ++iu) {
			float nu = static_cast<float>(iu) / u_segments - 0.5f;

			g.vertices.push_back(o + u * nu + v * nv);

			if (iu < u_segments && iv < v_segments) {
				g.indices.push_back(i);
				g.indices.push_back(i + 1);
				g.indices.push_back(i + 2 + u_segments);
				g.indices.push_back(i);
				g.indices.push_back(i + 2 + u_segments);
				g.indices.push_back(i + 1 + u_segments);
			}
			++i;
		}
	}

	return g;
}