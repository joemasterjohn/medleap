#include "SegmentRing.h"

using namespace std;
using namespace gl;

SegmentRing::SegmentRing(int segments, float inner_radius, float outer_radius) :
	segments_(segments),
	inner_radius_(inner_radius),
	outer_radius_(outer_radius)
{
}

Geometry SegmentRing::triangles()
{
	int divisions = 16;
	const float theta = two_pi / segments_;
	const float alpha = theta / 2.0f;
	const float phi = theta / (divisions - 1);
	const float width = 8.0f;
	const float half_width = width / 2.0f;
	const float d = half_width / sin(alpha);
	const float r1 = inner_radius_ - d;
	const float r2 = outer_radius_ - d;

	Geometry g;
	g.mode = GL_TRIANGLES;
	Vec2 p{ cos(alpha) * d, sin(alpha) * d };
	Vec2 v{ 1.0f, 0.0f };
	int k = 0;

	for (int i = 0; i < segments_; i++) {
		Vec2 u = v;
		for (int j = 0; j < divisions; j++) {
			g.vertices.push_back({ p + u * r1, 0.0f });
			g.vertices.push_back({ p + u * r2, 0.0f });

			if (j < divisions - 1) {
				g.indices.push_back(k + 0);
				g.indices.push_back(k + 1);
				g.indices.push_back(k + 3);
				g.indices.push_back(k + 0);
				g.indices.push_back(k + 3);
				g.indices.push_back(k + 2);
			}
			k += 2;
			u.rotate(phi);
		}
		v.rotate(theta);
		p.rotate(theta);
	}

	return g;
}