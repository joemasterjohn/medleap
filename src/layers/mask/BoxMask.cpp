#include "BoxMask.h"

using namespace std;
using namespace gl;

BoxMask::BoxMask(const Box box) : box_(box)
{
}


void BoxMask::center(const Vec3& center)
{
	box_.center(center);
}

Vec3 BoxMask::center() const
{
	return box_.center();
}

Geometry BoxMask::geometry() const
{
	return box_.lines();
}

MaskVolume::Edit BoxMask::apply(const Box& bounds, const Texture& texture, MaskVolume::Operation operation) const
{
	Vec3 min = bounds.normalize(bounds.clamp(box_.min()));
	Vec3 max = bounds.normalize(bounds.clamp(box_.max()));;

	if (min == max) {
		vector<Vec3i> voxels;
		return{ operation, voxels };
	}

	GLuint x1 = static_cast<GLuint>(min.x * texture.width());
	GLuint y1 = static_cast<GLuint>(min.y * texture.height());
	GLuint z1 = static_cast<GLuint>(min.z * texture.depth());

	GLuint x2 = static_cast<GLuint>(max.x * texture.width());
	GLuint y2 = static_cast<GLuint>(max.y * texture.height());
	GLuint z2 = static_cast<GLuint>(max.z * texture.depth());

	GLuint w = x2 - x1;
	GLuint h = y2 - y1;
	GLuint d = z2 - z1;

	GLuint total = w * h * d;
	if (total == 0) {
		vector<Vec3i> voxels;
		return{ operation, voxels };
	}

	vector<GLubyte> data;
	data.resize(total, static_cast<GLubyte>(255));
	texture.bind();
	glTexSubImage3D(GL_TEXTURE_3D, 0, x1, y1, z1, w, h, d, GL_RED, GL_UNSIGNED_BYTE, &data[0]);


	//for (GLuint z = x1; z < d; z++) {
	//	for (GLuint y = y1; y < h; y++) {
	//		for (GLuint x = z1; x < w; x++) {
	//			// keep in memory so I know?
	//		}
	//	}
	//}

	//// TODO: which voxels were affected?

	vector<Vec3i> voxels;


	return{ operation, voxels };
}