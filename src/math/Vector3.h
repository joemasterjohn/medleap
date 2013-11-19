#ifndef CGL_VECTOR3_H_
#define CGL_VECTOR3_H_

#include <iostream>
#include <iomanip>
#include "Math.h"

namespace cgl
{
  template <typename T> class Vector2;
  template <typename T> class Vector3;
  template <typename T> class Vector4;
  typedef Vector3<float> Vec3;
  typedef Vector3<double> Vec3d;
  
  /// 3D Vector with X, Y, and Z components.
  template <typename T> class Vector3 
  {
  public:

    /// The X component.
    T x;

    /// The Y component.
    T y;

    /// The Z component.
    T z;

    /// Constructs a 3D vector with all components set to s (default 0).
    Vector3<T>(T s = 0) : x(s), y(s), z(s) {}

    /// Constructs a 3D vector with components set to given values.
    Vector3<T>(T x, T y, T z) : x(x), y(y), z(z) {}

    /// Constructs a 3D vector by copying the first two values from v; z = 0.
    Vector3<T>(const Vector2<T>& v) : x(v.x), y(v.y), z(0) {}

    /// Constructs a 3D vector by copying the values from v.
    Vector3<T>(const Vector3<T>& v) : x(v.x), y(v.y), z(v.z) {}

    /// Constructs a 3D vector by copying the first three values from v.
    Vector3<T>(const Vector4<T>& v) : x(v.x), y(v.y), z(v.z) {}

    /// Constructs a 3D vector from an array.
    Vector3<T>(const T a[3]) : x(a[0]), y(a[1]), z(a[2]) {}

    /// Returns the number of components in the vector (3).
    unsigned size() const { return 3; }

    /// Treats the vector as an array of values.
    operator T*()
    {
      return &x;
    }

    /// Returns a reference to the component at index.
    T& operator[](int index)
    {
      return *(&x + index);
    }

    /// Returns a copy of the component at index.
    T operator[](int index) const
    {
      return *(&x + index);
    }

    /// Computes the length/magnitude squared of the vector.
    T lengthSquared() const
    {
      return x * x + y * y + z * z;
    }

    /// Computes the length/magnitude of the vector.
    T length() const
    {
      return std::sqrt(lengthSquared());
    }

    /// Compute the dot product.
    T dot(const Vector3<T>& v) const
    {
      return x * v.x + y * v.y + z * v.z;
    }

    /// Computes the 3D cross product.
    Vector3<T> cross(const Vector3<T>& v) const
    {
      return Vector3<T>(y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x);
    }

    /// Returns a copy of this vector normalized.
    Vector3<T> normal() const 
    {
      return (*this) / length();
    } 

    /// Scales the components to ensure the vector has unit length.
    Vector3<T>& normalize()
    {
      return (*this) /= length();
    }

    /// Returns the reflection of this vector on a surface with normal n.
    Vector3<T> reflect(const Vector3<T>& n) const
    {
      return (*this) - n * n.dot(*this) * 2;
    }

    /// Returns true if the two vectors have the exact same component values.
    bool operator==(const Vector3<T>& v) const
    {
      return x == v.x && y == v.y && z == v.z;
    }

    /// Returns true if the two vectors have at least one component that differs.
    bool operator!=(const Vector3<T>& v) const
    {
      return x != v.x || y != v.y || z != v.z;
    }

    /// Negates all components of this vector.
    Vector3<T> operator-() const
    {
      return Vector3<T>(-x, -y, -z);
    }

    /// Adds the scalar s to all components of this vector.
    Vector3<T> operator+(T s) const
    {
      return Vector3<T>(this->x + s, this->y + s, this->z + s);
    }

    /// Component-wise addition of this vector and v.
    Vector3<T> operator+(const Vector3<T>& v) const
    {
      return Vector3<T>(x + v.x, y + v.y, z + v.z);
    }

    /// Adds the scalar s to all components of the vector.
    Vector3<T>& operator+=(T s)
    { 
      x += s;
      y += s;
      z += s;
      return *this;
    } 

    /// Component-wise addition of this vector and v.
    Vector3<T>& operator+=(const Vector3<T>& v)
    { 
      x += v.x;
      y += v.y;
      z += v.z;
      return *this;
    }

    /// Subtracts the scalar s from all components of this vector.
    Vector3<T> operator-(T s) const
    {
      return Vector3<T>(this->x - s, this->y - s, this->z - s);
    }

    /// Component-wise subtraction of this vector and v.
    Vector3<T> operator-(const Vector3<T>& v) const
    {
      return Vector3<T>(x - v.x, y - v.y, z - v.z);
    }

    /// Subtracts the scalar s from all components of the vector.
    Vector3<T>& operator-=(T s)
    { 
      x -= s;
      y -= s;
      z -= s;
      return *this;
    } 

    /// Component-wise subtraction of this vector and v.
    Vector3<T>& operator-=(const Vector3<T>& v)
    { 
      x -= v.x;
      y -= v.y;
      z -= v.z;
      return *this;
    }

    /// Multiplies the scalar s with all components of this vector.
    Vector3<T> operator*(T s) const
    {
      return Vector3<T>(this->x * s, this->y * s, this->z * s);
    }

    /// Component-wise multiplication of this vector and v.
    Vector3<T> operator*(const Vector3<T>& v) const
    {
      return Vector3<T>(x * v.x, y * v.y, z * v.z);
    }

    /// Multiplies the scalar s with all components of the vector.
    Vector3<T>& operator*=(T s)
    { 
      x *= s;
      y *= s;
      z *= s;
      return *this;
    } 

    /// Component-wise multiplication of this vector and v.
    Vector3<T>& operator*=(const Vector3<T>& v)
    { 
      x *= v.x;
      y *= v.y;
      z *= v.z;
      return *this;
    }

    /// Divides all components of this vector by the scalar s.
    Vector3<T> operator/(T s) const
    {
      return Vector3<T>(this->x / s, this->y / s, this->z / s);
    }

    /// Component-wise division of this vector and v.
    Vector3<T> operator/(const Vector3<T>& v) const
    {
      return Vector3<T>(x / v.x, y / v.y, z / v.z);
    }

    /// Divides all components of this vector by the scalar s.
    Vector3<T>& operator/=(T s)
    { 
      x /= s;
      y /= s;
      z /= s;
      return *this;
    } 

    /// Component-wise division of this vector and v.
    Vector3<T>& operator/=(const Vector3<T>& v)
    { 
      x /= v.x;
      y /= v.y;
      z /= v.z;
      return *this;
    }

    /// Returns the zero vector (0,0,0).
    static const Vector3<T>& zero()
    { 
      static Vector3<T> s_zero;
      return s_zero;
    }

    /// Returns the one vector (1,1,1).
    static const Vector3<T>& one() 
    {
      static Vector3<T> s_one(1);
      return s_one; 
    }

    /// Returns the x-axis vector (1,0,0).
    static const Vector3<T>& xAxis()
    {
      static Vector3<T> s_x(1,0,0); 
      return s_x;
    }

    /// Returns the y-axis vector (0,1,0).
    static const Vector3<T>& yAxis()
    { 
      static Vector3<T> s_y(0,1,0);
      return s_y;
    }

    /// Returns the z-axis vector (0,0,1).
    static const Vector3<T>& zAxis()
    {
      static Vector3<T> s_z(0,0,1);
      return s_z;
    }

    /// Prints the vector to an output stream.
    friend std::ostream& operator<<(std::ostream& os, const Vector3<T>& v)
    {
      os << std::fixed;
      os << std::setprecision(3);
      os << "(";
      os << std::setw(10) << v.x << " ";
      os << std::setw(10) << v.y << " ";
      os << std::setw(10) << v.z << ")";
      return os;
    }
  };

}

#endif
