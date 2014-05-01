#include "MaskVolume.h"

using namespace gl;
using namespace std;

SphereMask::SphereMask(const Sphere sphere) : sphere_(sphere)
{
}

vector<Vec3i> SphereMask::apply(const Texture& texture, const Vec3& position, Operation operation) const
{
	vector<Vec3i> affected;

	return affected;
}


BoxMask::BoxMask(const Box box) : box_(box)
{
}

vector<Vec3i> BoxMask::apply(const Texture& texture, const Vec3& position, Operation operation) const
{
	vector<Vec3i> affected;

	return affected;
}