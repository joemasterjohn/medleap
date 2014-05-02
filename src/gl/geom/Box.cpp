#include "Box.h"
#include <algorithm>
#include "Plane.h"
#include "gl/math/Math.h"

using namespace gl;

Box::Box(float size) : Box({ 0.0f, 0.0f, 0.0f }, size, size, size) {}

Box::Box(float width, float height, float length) : Box({ 0.0f, 0.0f, 0.0f }, width, height, length) {}

Box::Box(const Vec3& center, float size) : Box(center, size, size, size) {}

Box::Box(const Vec3& center, float width, float height, float length) : center_(center)
{
	vertices_.resize(8, { 0.0f, 0.0f, 0.0f });
	size(width, height, length);

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

void Box::size(float size)
{
	Box::size(size, size, size);
}

void Box::size(float width, float height, float length)
{
	size_.set(width, height, length);

	Vec3 h = size_ / 2.0f;
	min_ = center_ - h;
	max_ = center_ + h;

	vertices_[0].set(center_.x - h.x, center_.y - h.y, center_.z + h.z);
	vertices_[1].set(center_.x + h.x, center_.y - h.y, center_.z + h.z);
	vertices_[2].set(center_.x + h.x, center_.y + h.y, center_.z + h.z);
	vertices_[3].set(center_.x - h.x, center_.y + h.y, center_.z + h.z);

	vertices_[4].set(center_.x - h.x, center_.y - h.y, center_.z - h.z);
	vertices_[5].set(center_.x + h.x, center_.y - h.y, center_.z - h.z);
	vertices_[6].set(center_.x + h.x, center_.y + h.y, center_.z - h.z);
	vertices_[7].set(center_.x - h.x, center_.y + h.y, center_.z - h.z);
}

void Box::center(const Vec3& center)
{
	center_ = center;
	size(size_.x, size_.y, size_.z);
}

Geometry Box::lines() const
{
	Geometry g;

	g.vertices = vertices_;

	for (const Box::Edge& e : edges_) {
		g.indices.push_back(e.first);
		g.indices.push_back(e.second);
	}

	return g;
}

Geometry Box::triangles(unsigned x_segments, unsigned y_segments, unsigned z_segments) const
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

bool Box::contains(const Vec3& p) const
{
	return (p.x >= min_.x && p.y <= max_.x &&
			p.y >= min_.y && p.y <= max_.y &&
			p.z >= min_.z && p.z <= max_.z);
}

Vec3 Box::normalize(const Vec3& p) const
{
	return (p - min_) / size_;
}

Vec3 Box::clamp(const Vec3& p) const
{
	float x = gl::clamp(p.x, min_.x, max_.x);
	float y = gl::clamp(p.y, min_.y, max_.y);
	float z = gl::clamp(p.z, min_.z, max_.z);
	return{ x, y, z };
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
					vertices.push_back(v);
				} else if (t == 1.0f && !corners[e.second]) {
					Vec3 v = a + d * t;
					corners[e.second] = true;
					vertices.push_back(v);
				} else {
					Vec3 v = a + d * t;
					vertices.push_back(v);
				}
			}
		}
	}

	// x and y are random orthogonal vectors in the plane
	Vec3 x = Vec3::random().cross(n).normalize();
	Vec3 y = n.cross(x);
	auto sort_angle = [&](const Vec3& v)->float {
		float s = (y.cross(v)).dot(n);
		return (v.cross(x).dot(n) >= 0.0f) ? 2.0f - s : s;
	};

	auto sort_comp = [&](const Vec3& a, const Vec3& b)->bool {
		return sort_angle(a.normal()) < sort_angle(b.normal());
	};

	std::sort(vertices.begin(), vertices.end(), sort_comp);

	return vertices;
}