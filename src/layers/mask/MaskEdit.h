#ifndef __medleap_MaskEdit__
#define __medleap_MaskEdit__

#include "MaskVolume.h"

class MaskEdit
{
public:
	MaskEdit(const gl::Texture& texture, const gl::Vec3& position, const MaskVolume& volume, MaskVolume::Operation operation)
	{
	}

	void redo()
	{
		switch (operation_)
		{
		case MaskVolume::Operation::sub:
			break;
		case MaskVolume::Operation::add:
			break;
		}
	}

	void undo()
	{
		switch (operation_)
		{
		case MaskVolume::Operation::sub:
			break;
		case MaskVolume::Operation::add:
			break;
		}
	}

private:
	MaskVolume::Operation operation_;
	std::vector<gl::Vec3i> voxels_;
};

#endif // __medleap_MaskEdit__
