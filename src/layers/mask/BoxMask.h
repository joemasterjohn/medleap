#ifndef __medleap_BoxMask__
#define __medleap_BoxMask__

#include "MaskVolume.h"
#include "gl/geom/Box.h"

/** Axis-aligned box masking volume */
class BoxMask : public MaskVolume
{
public:
	BoxMask(const gl::Box box);
	Edit apply(const gl::Box& bounds, const gl::Texture& texture, MaskVolume::Operation operation) const;
	gl::Geometry geometry() const override;
	void center(const gl::Vec3& center) override;
	gl::Vec3 center() const override;
	void scale(float scale) override;

private:
	gl::Box box_;
};

#endif // __medleap_BoxMask__
