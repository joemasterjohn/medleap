#ifndef __GL_MATH_H__
#define __GL_MATH_H__

#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace gl
{
	template <typename T> struct Vector2;
	template <typename T> struct Vector3;
	template <typename T> struct Vector4;
	template <typename T> class Matrix2;
	template <typename T> class Matrix3;
	template <typename T> class Matrix4;

	typedef Vector4<float>  Vec4;
	typedef Vector4<double> Vec4d;
	typedef Vector4<int>    Vec4i;
	typedef Vector3<float>  Vec3;
	typedef Vector3<double> Vec3d;
	typedef Vector3<int>    Vec3i;
	typedef Vector2<float>  Vec2;
	typedef Vector2<double> Vec2d;
	typedef Vector2<int>    Vec2i;

	typedef Matrix4<float> Mat4;
	typedef Matrix4<double> Mat4d;
	typedef Matrix3<float> Mat3;
	typedef Matrix3<double> Mat3d;
	typedef Matrix2<float> Mat2;
	typedef Matrix2<double> Mat2d;

	static const float pi = 3.1415926536f;
	static const float two_pi = pi * 2.0f;
	static const float pi_over_2 = pi / 2.0f;
	static const float pi_over_3 = pi / 3.0f;
	static const float pi_over_4 = pi / 4.0f;
	static const float pi_over_6 = pi / 6.0f;
	static const float deg_to_rad = 0.0174532925f;
	static const float rad_to_deg = 57.295779513f;

	/** Calculates the screen space radius of a sphere:
	  * fovY = vertical field of view used in the projection (radians)
	  * d = distance between camera center/eye to center of sphere
	  * r = radius of sphere in world space
	  */
	double projectedRadius(double fovY, double d, double r);

	// Returns the value clamped to be in [min, max].
	template <typename T>
	T clamp(T value, T min, T max)
	{
		return std::min(std::max(value, min), max);
	}

	// Linearly interoplate from a (p=0) to b (p=1)
	template <typename T>
	T lerp(const T& a, const T& b, float p)
	{
		return a + (b - a) * p;
	}

	template <typename T>
	static float normalize(T value) {
		T t_min = std::numeric_limits<T>::min();
		T t_max = std::numeric_limits<T>::max();
		return static_cast<float>(value - t_min) / (t_max - t_min);
	}

	template <typename T>
	static T unnormalize(float value) {
		T t_min = std::numeric_limits<T>::min();
		T t_max = std::numeric_limits<T>::max();
		return static_cast<T>(t_min + (t_max - t_min) * value);
	}

	// Creates a rotation matrix (X axis)
	Mat4 rotationX(float radians);

	// Creates a rotation matrix (Y axis)
	Mat4 rotationY(float radians);

	// Creates a rotation matrix (Z axis)
	Mat4 rotationZ(float radians);

	// Creates a rotation matrix (custom axis)
	Mat4 rotation(float radians, float x, float y, float z);

	// Creates a rotation matrix (custom axis)
	Mat4 rotation(float radians, const Vec3& axis);

	// Creates a translation matrix.
	Mat4 translation(float x, float y, float z);

	// Creates a translation matrix.
	Mat4 translation(const gl::Vec3& t);

	// Creates a scale matrix.
	Mat4 scale(float x, float y, float z);

	// Creates a scale matrix.
	Mat4 scale(const Vec3& s);

	// Creates a perspective projection matrix.
	Mat4 perspective(float left, float right, float bottom, float top, float zNear, float zFar);

	// Creates a perspective projection matrix.
	Mat4 perspective(float fovRadians, float aspect, float zNear, float zFar);

	// Creates an orthographic projection matrix.
	Mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar);

	// Creates a 2D orthographic projection matrix (zNear = -1, zFar = +1).
	Mat4 ortho2D(float left, float right, float bottom, float top);

	// Creates a viewing matrix that positions the camera at eye looking at center.
	Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);

	// Creates a viewing matrix that positions the camera at eye looking at center.
	Mat4 lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);
}

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix4.h"

#endif // __GL_MATH_H__