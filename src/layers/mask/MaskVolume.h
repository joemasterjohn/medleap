#ifndef __medleap_MaskVolume__
#define __medleap_MaskVolume__

#include "gl/math/Math.h"
#include "gl/geom/Sphere.h"
#include "gl/geom/Box.h"

/** 3D space that can be subtracted/added with a volume texture */
class MaskVolume
{
public:
	enum class Operation { sub, add };

	class Edit
	{
	public:
		Edit();
		Edit(Operation operation, std::vector<gl::Vec3i>& voxels);
		void redo(gl::Texture& texture);
		void undo(gl::Texture& texture);

	private:
		Operation operation_;
		std::vector<gl::Vec3i> voxels_;
	};

	virtual ~MaskVolume();

	/** Applies operation on texture centered at position. */
	virtual Edit apply(gl::Texture& texture, const gl::Vec3& position, Operation operation) const = 0;
};

/** Spherical masking volume */
class SphereMask : public MaskVolume
{
public:
	SphereMask(const gl::Sphere sphere);
	Edit apply(gl::Texture& texture, const gl::Vec3& position, Operation operation) const;

private:
	gl::Sphere sphere_;
};

/** Axis-aligned box masking volume */
class BoxMask : public MaskVolume
{
public:
	BoxMask(const gl::Box box);
	Edit apply(gl::Texture& texture, const gl::Vec3& position, Operation operation) const;

private:
	gl::Box box_;
};

#endif // __medleap_MaskVolume__
