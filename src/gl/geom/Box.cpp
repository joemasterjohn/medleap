#include "Box.h"
#include <algorithm>
#include "Plane.h"
#include "gl/math/Math.h"

using namespace gl;

Box::Box(GLfloat edgeLength, unsigned segments) : Box(edgeLength, edgeLength, edgeLength, segments, segments, segments)
{
}

Box::Box(GLfloat width, GLfloat height, GLfloat depth, unsigned widthSegments, unsigned heightSegments, unsigned depthSegments)
{
	auto addPlane = [&](const Plane& plane, const Mat4& m, const Vec3& offset) {
		size_t indexOffset = vertices.size();
		for (GLuint i : plane.getIndices())
			indices.push_back(static_cast<GLuint>(i + indexOffset));
		for (const Vec3& v : plane.getVertices())
			vertices.push_back(Vec3(m * Vec4(v.x, v.y, v.z, 1.0f)) + offset);
	};

	addPlane(Plane(depth, height, depthSegments, heightSegments), rotationY(pi / 2.0f), Vec3(+width / 2.0f, 0, 0));
	addPlane(Plane(depth, height, depthSegments, heightSegments), rotationY(-pi / 2.0f), Vec3(-width / 2.0f, 0, 0));
	addPlane(Plane(width, depth, widthSegments, depthSegments), rotationX(-pi / 2.0f), Vec3(0.0f, +height / 2.0f, 0.0f));
	addPlane(Plane(width, depth, widthSegments, depthSegments), rotationX(pi / 2.0f), Vec3(0.0f, -height / 2.0f, 0.0f));
	addPlane(Plane(width, height, widthSegments, heightSegments), Mat4(), Vec3(0.0f, 0.0f, +depth / 2.0f));
	addPlane(Plane(width, height, widthSegments, heightSegments), rotationY(pi), Vec3(0.0f, 0.0f, -depth / 2.0f));
}