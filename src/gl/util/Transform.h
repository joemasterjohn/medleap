#ifndef __GL_UTIL_TRANSFORM_H__
#define __GL_UTIL_TRANSFORM_H__

#include "gl/math/Math.h"

namespace gl
{
	class Transform
	{
	public:
		Transform();
		virtual ~Transform();

		const Mat4& matrix() const;
		const Mat4& inverse() const;
		const Vec3& position() const;
		const Vec3& x() const;
		const Vec3& y() const;
		const Vec3& z() const;

		void matrix(const Mat4& matrix);
		void position(const Vec3& position);

	protected:
		Mat4 matrix_;
		Mat4 inverse_;
		Vec3 position_;
		Vec3 x_;
		Vec3 y_;
		Vec3 z_;

		void update();
	};


	class YawPitchTransform : public Transform
	{
	public:
		float yaw() const;
		float pitch() const;

		void yaw(float yaw);
		void pitch(float pitch);
		void yawPitch(float yaw, float pitch);

	private:
		float yaw_;
		float pitch_;
	};
}

#endif // __GL_UTIL_TRANSFORM_H__