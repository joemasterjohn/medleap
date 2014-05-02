#include "SphereMask.h"

using namespace gl;
using namespace std;

SphereMask::SphereMask(const Sphere sphere) : sphere_(sphere)
{
}

void SphereMask::center(const Vec3& center)
{
	sphere_.center(center);
}

Vec3 SphereMask::center() const
{
	return sphere_.center();
}

Geometry SphereMask::geometry() const
{
	return sphere_.triangles(8);
}

MaskVolume::Edit SphereMask::apply(const Box& bounds, const Texture& texture, Operation operation) const
{
	vector<Vec3i> voxels;


	return{ operation, voxels };
}
