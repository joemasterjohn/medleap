#include "Math.h"

namespace gl
{
	double projectedRadius(double fovY, double d, double r) {
		// http://stackoverflow.com/questions/21648630/radius-of-projected-sphere-in-screen-space?lq=1
		return 1.0 / std::tan(fovY / 2.0) * r / std::sqrt(d * d - r * r);
	}

	Mat4 rotationX(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);
		return Mat4(1, 0, 0, 0, 0, c, s, 0, 0, -s, c, 0, 0, 0, 0, 1);
	}

	Mat4 rotationY(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);
		return Mat4(c, 0, -s, 0, 0, 1, 0, 0, s, 0, c, 0, 0, 0, 0, 1);
	}

	Mat4 rotationZ(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);
		return Mat4(c, s, 0, 0, -s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	}

	Mat4 rotation(float radians, float x, float y, float z)
	{
		float c = cos(radians);
		float s = sin(radians);
		float u2 = x * x;
		float v2 = y * y;
		float w2 = z * z;
		float d = u2 + v2 + w2;
		float ic = 1 - c;
		float sqrtDs = sqrt(d) * s;

		return Mat4(
			(u2 + (v2 + w2) * c) / d,
			(x * y * ic + z * sqrtDs) / d,
			(x * z * ic - y * sqrtDs) / d,
			0,
			(x * y * ic - z * sqrtDs) / d,
			(v2 + (u2 + w2) * c) / d,
			(y * z * ic + x * sqrtDs) / d,
			0,
			(x * z * ic + y * sqrtDs) / d,
			(y * z * ic - x * sqrtDs) / d,
			(w2 + (u2 + v2) * c) / d,
			0,
			0, 0, 0, 1);
	}

	Mat4 rotation(float radians, const Vec3& axis)
	{
		return rotation(radians, axis.x, axis.y, axis.z);
	}

	Mat4 translation(float x, float y, float z)
	{
		return Mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1);
	}

	Mat4 translation(const gl::Vec3& t)
	{
		return Mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, t.x, t.y, t.z, 1);
	}

	Mat4 scale(float x, float y, float z)
	{
		return Mat4(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
	}

	Mat4 scale(const Vec3& s)
	{
		return Mat4(s.x, 0, 0, 0, 0, s.y, 0, 0, 0, 0, s.z, 0, 0, 0, 0, 1);
	}

	Mat4 perspective(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		float w = right - left;
		float h = top - bottom;
		float d = zFar - zNear;
		float n2 = zNear * 2.f;

		float A = (right + left) / w;
		float B = (top + bottom) / h;
		float C = -(zFar + zNear) / d;
		float D = -n2 * zFar / d;

		return Mat4(n2 / w, 0, 0, 0, 0, n2 / h, 0, 0, A, B, C, -1, 0, 0, D, 0);
	}

	Mat4 perspective(float fovRadians, float aspect, float zNear, float zFar)
	{
		float f = 1.f / std::tan(fovRadians / 2.f);
		float d = zNear - zFar;
		return Mat4(f / aspect, 0, 0, 0, 0, f, 0, 0, 0, 0, (zFar + zNear) / d, -1, 0, 0, (2.f * zFar * zNear) / d, 0);
	}

	Mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		float w = right - left;
		float h = top - bottom;
		float d = zFar - zNear;

		float tx = -(right + left) / w;
		float ty = -(top + bottom) / h;
		float tz = -(zFar + zNear) / d;

		return Mat4(2.f / w, 0, 0, 0, 0, 2.f / h, 0, 0, 0, 0, -2.f / d, 0, tx, ty, tz, 1.f);
	}

	Mat4 ortho2D(float left, float right, float bottom, float top)
	{
		return ortho(left, right, bottom, top, -1.f, 1.f);
	}

	Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up)
	{
		Vec3 c_forward = (center - eye).normalize();
		Vec3 c_right = c_forward.cross(up).normalize();
		Vec3 c_up = c_right.cross(c_forward).normalize();

		return Mat4(c_right.x, c_up.x, -c_forward.x, 0,
			c_right.y, c_up.y, -c_forward.y, 0,
			c_right.z, c_up.z, -c_forward.z, 0,
			0, 0, 0, 1) * translation(-eye);
	}

	Mat4 lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
	{
		return lookAt(Vec3(eyeX, eyeY, eyeZ), Vec3(centerX, centerY, centerZ), Vec3(upX, upY, upZ));
	}
}