#include "MaskVolume.h"
#include "main/MainController.h"

using namespace gl;
using namespace std;

MaskVolume::Edit::Edit() {}

MaskVolume::Edit::Edit(MaskVolume::Operation operation, std::vector<gl::Vec3i>& voxels) :
operation_(operation),
voxels_(std::move(voxels)) {}

MaskVolume::Edit::Edit(Edit&& edit)
{
	operation_ = edit.operation_;
	std::swap(voxels_, edit.voxels_);
}

MaskVolume::Edit& MaskVolume::Edit::operator=(Edit&& edit)
{
    operation_ = edit.operation_;
	std::swap(voxels_, edit.voxels_);
    return *this;
}

MaskVolume::Edit::Edit(const Edit& edit)
{
    operation_ = edit.operation_;
    voxels_ = edit.voxels_;
}

MaskVolume::Edit& MaskVolume::Edit::operator=(const Edit& edit)
{
    operation_ = edit.operation_;
    voxels_ = edit.voxels_;
    return *this;
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



