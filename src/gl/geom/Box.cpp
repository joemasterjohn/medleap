#include "Box.h"
#include <algorithm>
#include "Plane.h"
#include "gl/math/Math.h"

using namespace gl;

Box::Box(float size) : Box(size, size, size)
{
}

Box::Box(float width, float height, float length) : size_({ width, height, length })
{
	float hw = width / 2.0f;
	float hh = height / 2.0f;
	float hl = length / 2.0f;

	vertices_.push_back(Vec4(-hw, -hh, hl, 1));
	vertices_.push_back(Vec4(hw, -hh, hl, 1));
	vertices_.push_back(Vec4(hw, hh, hl, 1));
	vertices_.push_back(Vec4(-hw, hh, hl, 1));
	vertices_.push_back(Vec4(-hw, -hh, -hl, 1));
	vertices_.push_back(Vec4(hw, -hh, -hl, 1));
	vertices_.push_back(Vec4(hw, hh, -hl, 1));
	vertices_.push_back(Vec4(-hw, hh, -hl, 1));

	edges_.push_back(Box::Edge(0, 1));
	edges_.push_back(Box::Edge(1, 2));
	edges_.push_back(Box::Edge(2, 3));
	edges_.push_back(Box::Edge(3, 0));
	edges_.push_back(Box::Edge(4, 5));
	edges_.push_back(Box::Edge(5, 6));
	edges_.push_back(Box::Edge(6, 7));
	edges_.push_back(Box::Edge(7, 4));
	edges_.push_back(Box::Edge(0, 4));
	edges_.push_back(Box::Edge(1, 5));
	edges_.push_back(Box::Edge(2, 6));
	edges_.push_back(Box::Edge(3, 7));
}

Geometry Box::lines()
{
	Geometry g;



	return g;
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

Vec3 Box::min() const
{
	return -size_ / 2.0f;
}

Vec3 Box::max() const
{
	return size_ / 2.0f;
}

std::vector<Vec3> Box::intersect(const Plane& plane) const
{
	std::vector<Vec3> vertices;

	Vec3 n = plane.normal();
	Vec3 p = plane.nearestToOrigin();

	// to avoid duplicate vertices at corners, only the first vertex that
	// intersects a corner vertex i will be added (i.e. corners[i] == false)
	bool corners[] = { false, false, false, false, false, false, false, false };

	// find intersections of the plane with all edges
	for (const Box::Edge& e : edges_) {
		const Vec3& a = vertices_[e.first];
		const Vec3& b = vertices_[e.second];
		Vec3 d = b - a;

		// if n * d == 0, the edge lies on the plane (ignore it)
		GLfloat nDotD = n.dot(d);
		if (nDotD != 0) {
			float t = -n.dot(a - p) / nDotD;
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

	return v;
}