#ifndef CGL_MATH_H_
#define CGL_MATH_H_

#include <cmath>
#include <algorithm>

namespace cgl
{
    static const float PI          = 3.1415926536f;
    static const float PI2         = 2.0f * PI;
    static const float DEG_TO_RAD  = 0.0174532925f;
    static const float RAD_TO_DEG  = 57.295779513f;
    
    template <typename T> class Vector2;
    template <typename T> class Vector3;
    template <typename T> class Vector4;
    template <typename T> class Matrix2;
    template <typename T> class Matrix3;
    template <typename T> class Matrix4;

    typedef Vector2<float> Vec2;
    typedef Vector2<double> Vec2d;
    typedef Vector3<float> Vec3;
    typedef Vector3<double> Vec3d;
    typedef Vector4<float> Vec4;
    typedef Vector4<double> Vec4d;
    typedef Matrix2<float> Mat2;
    typedef Matrix2<double> Mat2d;
    typedef Matrix3<float> Mat3;
    typedef Matrix3<double> Mat3d;
    typedef Matrix4<float> Mat4;
    typedef Matrix4<double> Mat4d;
}

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix4.h"

namespace cgl {
  
  /// Returns the value clamped to be in [min, max].
  template <typename T> T clamp(T value, T min, T max)
  {
    return std::min(std::max(value, min), max);
  }

  /// Returns the linear interpolation of a and b given a weight w. Returns a
  /// when w == 0, and b when w == 1; if w is not in [0,1] it extrapolates.
  template <typename T> T lerp(const T& a, const T& b, float w)
  {
    return a + (b - a) * w;
  }

  /// Creates a rotation matrix (X axis)
  inline  Mat4 rotationX(float radians)
  {
    float c = std::cos(radians);
    float s = std::sin(radians);
    return Mat4(1, 0, 0, 0,   0, c, s, 0,   0, -s, c, 0,   0, 0, 0, 1);
  }
  
  /// Creates a rotation matrix (Y axis)
  inline  Mat4 rotationY(float radians)
  {
    float c = std::cos(radians);
    float s = std::sin(radians);
    return Mat4(c, 0, -s, 0,   0, 1, 0, 0,   s, 0, c, 0,   0, 0, 0, 1);
  }
  
  /// Creates a rotation matrix (Z axis)
  inline  Mat4 rotationZ(float radians)
  {
    float c = std::cos(radians);
    float s = std::sin(radians);
    return Mat4(c, s, 0, 0,   -s, c, 0, 0,   0, 0, 1, 0,   0, 0, 0, 1);
  }
  
  /// Creates a rotation matrix (custom axis)
  inline  Mat4 rotation(float radians, float x, float y, float z)
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
  
  /// Creates a rotation matrix (custom axis)
  inline  Mat4 rotation(float radians, const Vec3& axis)
  {
    return rotation(radians, axis.x, axis.y, axis.z);
  }
  
  /// Creates a translation matrix.
  inline  Mat4 translation(float x, float y, float z)
  {
    return Mat4(1, 0, 0, 0,   0, 1, 0, 0,   0, 0, 1, 0,   x, y, z, 1);
  }
  
  /// Creates a translation matrix.
  inline  Mat4 translation(const Vec3& t)
  {
    return Mat4(1, 0, 0, 0,   0, 1, 0, 0,   0, 0, 1, 0,   t.x, t.y, t.z, 1);
  }
  
  /// Creates a scale matrix.
  inline  Mat4 scale(float x, float y, float z)
  {
    return Mat4(x, 0, 0, 0,   0, y, 0, 0,   0, 0, z, 0,   0, 0, 0, 1);
  }
  
  /// Creates a scale matrix.
  inline Mat4 scale(const Vec3& s)
  {
    return Mat4(s.x, 0, 0, 0,   0, s.y, 0, 0,   0, 0, s.z, 0,   0, 0, 0, 1);
  }
  
  /// Creates a perspective projection matrix.
  inline Mat4 perspective(float left, float right, float bottom, float top, float zNear, float zFar)
  {
    float w = right - left;
    float h = top - bottom;
    float d = zFar - zNear;
    float n2 = zNear * 2.f;
    
    float A = (right + left) / w;
    float B = (top + bottom) / h;
    float C = -(zFar + zNear) / d;
    float D = -n2 * zFar / d;
    
    return Mat4(n2 / w, 0, 0, 0,   0, n2 / h, 0, 0,   A, B, C, -1,   0, 0, D, 0);
  }
  
  /// Creates a perspective projection matrix.
  inline Mat4 perspective(float fovRadians, float aspect, float zNear, float zFar)
  {
    float f = 1.f / std::tan(fovRadians / 2.f);
    float d = zNear - zFar;
    return Mat4(f / aspect, 0, 0, 0,   0, f, 0, 0,   0, 0, (zFar + zNear) / d, -1,   0, 0, (2.f * zFar * zNear) / d, 0);
  }
  
  /// Creates an orthographic projection matrix.
  inline Mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar)
  {
    float w = right - left;
    float h = top - bottom;
    float d = zFar - zNear;
    
    float tx = -(right + left) / w;
    float ty = -(top + bottom) / h;
    float tz = -(zFar + zNear) / d;
    
    return Mat4(2.f / w, 0, 0, 0,   0, 2.f / h, 0, 0,   0, 0, -2.f / d, 0,   tx, ty, tz, 1.f);
  }
  
  /// Creates a 2D orthographic projection matrix (zNear = -1, zFar = +1).
  inline Mat4 ortho2D(float left, float right, float bottom, float top)
  {
    return ortho(left, right, bottom, top, -1.f, 1.f);
  }
  
  /// Creates a viewing matrix that positions the camera at eye looking at center.
  inline Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up)
  {
    Vec3 c_forward = (center - eye).normalize();
    Vec3 c_right = c_forward.cross(up).normalize();
    Vec3 c_up = c_right.cross(c_forward).normalize();
    
    return Mat4(c_right.x, c_up.x, -c_forward.x, 0,
                c_right.y, c_up.y, -c_forward.y, 0,
                c_right.z, c_up.z, -c_forward.z, 0,
                0, 0, 0, 1) * translation(-eye);
  }
  
  /// Creates a viewing matrix that positions the camera at eye looking at center.
  inline Mat4 lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
  {
    return lookAt(Vec3(eyeX, eyeY, eyeZ), Vec3(centerX, centerY, centerZ), Vec3(upX, upY, upZ));
  }

} // namespace cgl

#endif
