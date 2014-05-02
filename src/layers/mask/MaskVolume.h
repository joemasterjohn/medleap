#ifndef __medleap_MaskVolume__
#define __medleap_MaskVolume__

#include "gl/math/Math.h"
#include "gl/geom/Sphere.h"
#include "gl/geom/Box.h"
#include "gl/util/Geometry.h"

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
		Edit(Edit&& edit);

		bool empty() { return voxels_.empty(); }
		void redo(gl::Texture& texture);
		void undo(gl::Texture& texture);

	private:
		Operation operation_;
		std::vector<gl::Vec3i> voxels_;
	};

	virtual ~MaskVolume();

	virtual Edit apply(gl::Texture& texture, Operation operation) const = 0;
	virtual gl::Geometry geometry() const = 0;
	virtual void center(const gl::Vec3& center) = 0;
	virtual gl::Vec3 center() const = 0;
};

/** Spherical masking volume */
class SphereMask : public MaskVolume
{
public:
	SphereMask(const gl::Sphere sphere);
	Edit apply(gl::Texture& texture, Operation operation) const;
	gl::Geometry geometry() const override;
	void center(const gl::Vec3& center) override;
	gl::Vec3 center() const override;

private:
	gl::Sphere sphere_;
};

/** Axis-aligned box masking volume */
class BoxMask : public MaskVolume
{
public:
	BoxMask(const gl::Box box);
	Edit apply(gl::Texture& texture, Operation operation) const;
	gl::Geometry geometry() const override;
	void center(const gl::Vec3& center) override;
	gl::Vec3 center() const override;

private:
	gl::Box box_;
};

#endif // __medleap_MaskVolume__
