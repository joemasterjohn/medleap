#include "Box.h"
#include <algorithm>
#include "Plane.h"
#include "gl/math/Math.h"

using namespace gl;

Box::Box(float size) : size_(size)
{
}

Box::Box(float width, float height, float length) : size_({ width, height, length })
{
}

Geometry Box::triangles(unsigned x_segments, unsigned y_segments, unsigned z_segments)
{
	Geometry g;
	g.mode = GL_TRIANGLES;

	auto addPlane = [&](const Vec3& n, unsigned u_segments, unsigned v_segments)
	{
		Geometry p = Plane(n, 0.5f).triangles(u_segments, v_segments);
		int offset = g.vertices.size();
		g.vertices.insert(g.vertices.end(), p.vertices.begin(), p.vertices.end());
		for (int i = 0; i < p.indices.size(); i++) {
			g.indices.push_back(p.indices[i] + offset);
		}
	};

	addPlane(Vec3{ +1.0f, +0.0f, +0.0f }, z_segments, y_segments);
	addPlane(Vec3{ -1.0f, +0.0f, +0.0f }, z_segments, y_segments);
	addPlane(Vec3{ +0.0f, +1.0f, +0.0f }, x_segments, z_segments);
	addPlane(Vec3{ +0.0f, -1.0f, +0.0f }, x_segments, z_segments);
	addPlane(Vec3{ +0.0f, +0.0f, +1.0f }, x_segments, y_segments);
	addPlane(Vec3{ +0.0f, +0.0f, -1.0f }, x_segments, y_segments);

	return g;
}