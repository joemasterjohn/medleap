#ifndef __medleap_MaskVolume__
#define __medleap_MaskVolume__

#include "gl/math/Math.h"
#include "gl/util/Geometry.h"
#include "gl/geom/Box.h"

/** 3D space that can be subtracted/added with a volume */
class MaskVolume
{
public:
	enum class Operation
	{ 
		sub, 
		add
	};

	class Edit
	{
	public:
		Edit();
		Edit(Operation operation, std::vector<gl::Vec3i>& voxels);
		Edit(Edit&& edit);

		bool empty() { return voxels_.empty(); }
		void redo(gl::Texture& texture);
		void undo(gl::Texture& texture);

	private:
		Operation operation_;
		std::vector<gl::Vec3i> voxels_;
	};

	virtual ~MaskVolume();
	virtual Edit apply(const gl::Box& bounds, const gl::Texture& texture, Operation operation) const = 0;
	virtual gl::Geometry geometry() const = 0;
	virtual void center(const gl::Vec3& center) = 0;
	virtual gl::Vec3 center() const = 0;
	virtual void scale(float scale) = 0;
};

#endif // __medleap_MaskVolume__
