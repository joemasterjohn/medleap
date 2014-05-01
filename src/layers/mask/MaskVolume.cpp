#include "MaskVolume.h"

using namespace gl;
using namespace std;

MaskVolume::Edit::Edit()
{
}

MaskVolume::Edit::Edit(MaskVolume::Operation operation, std::vector<gl::Vec3i>& voxels) :
operation_(operation),
voxels_(std::move(voxels))
{
}

void MaskVolume::Edit::redo(Texture& texture)
{
	switch (operation_)
	{
	case MaskVolume::Operation::sub:
		break;
	case MaskVolume::Operation::add:
		break;
	}
}

void MaskVolume::Edit::undo(Texture& texture)
{
	switch (operation_)
	{
	case MaskVolume::Operation::sub:
		break;
	case MaskVolume::Operation::add:
		break;
	}
}

MaskVolume::~MaskVolume() {}

SphereMask::SphereMask(const Sphere sphere) : sphere_(sphere)
{
}

MaskVolume::Edit SphereMask::apply(Texture& texture, const Vec3& position, Operation operation) const
{
	vector<Vec3i> voxels;

	return { operation, voxels };
}


BoxMask::BoxMask(const Box box) : box_(box)
{
}

MaskVolume::Edit BoxMask::apply(Texture& texture, const Vec3& position, Operation operation) const
{
	vector<Vec3i> voxels;

	return { operation, voxels };
}