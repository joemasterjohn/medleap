#include "Plane.h"

using namespace gl;

Plane::Plane(float width, float height, unsigned widthSegments, unsigned heightSegments)
{
	unsigned numVertices = (widthSegments + 1) * (heightSegments + 1);
	unsigned numIndices = widthSegments * heightSegments * 6;

	unsigned i = 0;
	for (unsigned iy = 0; iy <= heightSegments; ++iy) {
		float normalizedY = static_cast<float>(iy) / heightSegments;
		for (unsigned ix = 0; ix <= widthSegments; ++ix) {
			float normalizedX = static_cast<float>(ix) / widthSegments;

			float x = (normalizedX - 0.5f) * width;
			float y = (normalizedY - 0.5f) * height;
			vertices.push_back(Vec3(x, y, 0));

			if (ix < widthSegments && iy < heightSegments) {
				indices.push_back(i);
				indices.push_back(i + 1);
				indices.push_back(i + 2 + widthSegments);
				indices.push_back(i);
				indices.push_back(i + 2 + widthSegments);
				indices.push_back(i + 1 + widthSegments);
			}
			++i;
		}
	}
}