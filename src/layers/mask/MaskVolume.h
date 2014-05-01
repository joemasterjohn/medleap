#ifndef __medleap_MaskVolume__
#define __medleap_MaskVolume__

#include "gl/math/Math.h"
#include "gl/geom/Sphere.h"
#include "gl/geom/Box.h"
#include "MaskEdit.h"

/** 3D space that can be subtracted/added with a volume texture */
class MaskVolume
{
public:
	enum class Operation { sub, add };

	virtual ~MaskVolume() {}

	/** Applies operation on texture centered at position. */
	virtual MaskEdit apply(const gl::Texture& texture, const gl::Vec3& position, Operation operation) const = 0;
};

/** Spherical masking volume */
class SphereMask : MaskVolume
{
public:
	SphereMask(const gl::Sphere sphere);
	MaskEdit apply(const gl::Texture& texture, const gl::Vec3& position, Operation operation) const;

private:
	gl::Sphere sphere_;
};

/** Axis-aligned box masking volume */
class BoxMask : MaskVolume
{
public:
	BoxMask(const gl::Box box);
	MaskEdit apply(const gl::Texture& texture, const gl::Vec3& position, Operation operation) const;

private:
	gl::Box box_;
};

#endif // __medleap_MaskVolume__
