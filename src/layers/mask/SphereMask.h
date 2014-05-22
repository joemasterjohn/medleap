#ifndef __medleap_SphereMask__
#define __medleap_SphereMask__

#include "MaskVolume.h"
#include "gl/geom/Sphere.h"

/** Spherical masking volume */
class SphereMask : public MaskVolume
{
public:
	SphereMask(const gl::Sphere sphere);
	Edit apply(const gl::Box& bounds, const gl::Texture& texture, Operation operation) const;
	gl::Geometry geometry() const override;
	void center(const gl::Vec3& center) override;
	gl::Vec3 center() const override;
	void scale(float scale) override {}

private:
	gl::Sphere sphere_;
};

#endif // __medleap_SphereMask__
