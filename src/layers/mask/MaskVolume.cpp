#include "MaskVolume.h"
#include "main/MainController.h"

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

MaskVolume::Edit::Edit(Edit&& edit)
{
	operation_ = edit.operation_;
	voxels_ = std::move(edit.voxels_);
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

MaskVolume::Edit SphereMask::apply(Texture& texture, Operation operation) const
{
	vector<Vec3i> voxels;


	return { operation, voxels };
}


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

MaskVolume::Edit BoxMask::apply(Texture& texture, Operation operation) const
{
	// TODO: give volumedata instead of volume texture?
	vector<Vec3i> voxels;

	VolumeData* data = MainController::getInstance().volumeData();

	Vec3 min = data->getBounds().normalize(box_.min());
	Vec3 max = data->getBounds().normalize(box_.max());

	int x = static_cast<int>(min.x * data->getWidth());
	int y = static_cast<int>(min.y * data->getHeight());
	int z = static_cast<int>(min.z * data->getDepth());
	int w = static_cast<int>((max.x - min.x) * data->getWidth());
	int h = static_cast<int>((max.x - min.x) * data->getWidth());
	int d = static_cast<int>((max.x - min.x) * data->getWidth());

	// be careful not outside range

	vector<GLubyte> dat;
	dat.resize(w*h*d, static_cast<GLubyte>(255));
	texture.bind();
	glTexSubImage3D(GL_TEXTURE_3D, 0, x, y, z, w, h, d, GL_RED, GL_UNSIGNED_BYTE, &dat[0]);


	return { operation, voxels };
}