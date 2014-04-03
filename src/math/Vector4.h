#ifndef CGL_VECTOR4_H_
#define CGL_VECTOR4_H_

#include <iostream>
#include <iomanip>
#include <cmath>


/// 4D Vector with X, Y, Z, and W components.
template <typename T> class Vector4
{
public:
    
    /// The X component.
    T x;
    
    /// The Y component.
    T y;
    
    /// The Z component.
    T z;
    
    /// The W component.
    T w;
    
    /// Constructs a 4D vector with all components set to s (default 0).
    Vector4<T>(T s = 0) : x(s), y(s), z(s), w(s) {}
    
    /// Constructs a 4D vector with components set to given values.
    Vector4<T>(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    
    /// Constructs a 4D vector by copying the values from v.
    Vector4<T>(const Vector4<T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
    
    /// Constructs a 4D vector from an array.
    Vector4<T>(const T a[4]) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}
    
    /// Returns the number of components in the vector (4).
    unsigned size() const { return 4; }
    
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
        return x * x + y * y + z * z + w * w;
    }
    
    /// Computes the length/magnitude of the vector.
    T length() const
    {
        return std::sqrt(lengthSquared());
    }
    
    /// Compute the dot product.
    T dot(const Vector4<T>& v) const
    {
        return x * v.x + y * v.y + z * v.z + w * v.w;
    }
    
    /// Returns a copy of this vector normalized.
    Vector4<T> normal() const
    {
        return (*this) / length();
    }
    
    /// Scales the components to ensure the vector has unit length.
    Vector4<T>& normalize()
    {
        return (*this) /= length();
    }
    
    /// Returns true if the two vectors have the exact same component values.
    bool operator==(const Vector4<T>& v) const
    {
        return x == v.x && y == v.y && z == v.z && w == v.w;
    }
    
    /// Returns true if the two vectors have at least one component that differs.
    bool operator!=(const Vector4<T>& v) const
    {
        return x != v.x || y != v.y || z != v.z || w != v.w;
    }
    
    /// Negates all components of this vector.
    Vector4<T> operator-() const
    {
        return Vector4<T>(-x, -y, -z, -w);
    }
    
    /// Adds the scalar s to all components of this vector.
    Vector4<T> operator+(T s) const
    {
        return Vector4<T>(this->x + s, this->y + s, this->z + s, this->w + s);
    }
    
    /// Component-wise addition of this vector and v.
    Vector4<T> operator+(const Vector4<T>& v) const
    {
        return Vector4<T>(x + v.x, y + v.y, z + v.z, w + v.w);
    }
    
    /// Adds the scalar s to all components of the vector.
    Vector4<T>& operator+=(T s)
    {
        x += s;
        y += s;
        z += s;
        w += w;
        return *this;
    }
    
    /// Component-wise addition of this vector and v.
    Vector4<T>& operator+=(const Vector4<T>& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }
    
    /// Subtracts the scalar s from all components of this vector.
    Vector4<T> operator-(T s) const
    {
        return Vector4<T>(this->x - s, this->y - s, this->z - s, this->w - s);
    }
    
    /// Component-wise subtraction of this vector and v.
    Vector4<T> operator-(const Vector4<T>& v) const
    {
        return Vector4<T>(x - v.x, y - v.y, z - v.z, w - v.w);
    }
    
    /// Subtracts the scalar s from all components of the vector.
    Vector4<T>& operator-=(T s)
    {
        x -= s;
        y -= s;
        z -= s;
        w -= s;
        return *this;
    }
    
    /// Component-wise subtraction of this vector and v.
    Vector4<T>& operator-=(const Vector4<T>& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }
    
    /// Multiplies the scalar s with all components of this vector.
    Vector4<T> operator*(T s) const
    {
        return Vector4<T>(this->x * s, this->y * s, this->z * s, this->w * s);
    }
    
    /// Component-wise multiplication of this vector and v.
    Vector4<T> operator*(const Vector4<T>& v) const
    {
        return Vector4<T>(x * v.x, y * v.y, z * v.z, w * v.w);
    }
    
    /// Multiplies the scalar s with all components of the vector.
    Vector4<T>& operator*=(T s)
    {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }
    
    /// Component-wise multiplication of this vector and v.
    Vector4<T>& operator*=(const Vector4<T>& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }
    
    /// Divides all components of this vector by the scalar s.
    Vector4<T> operator/(T s) const
    {
        return Vector4<T>(this->x / s, this->y / s, this->z / s, this->w / s);
    }
    
    /// Component-wise division of this vector and v.
    Vector4<T> operator/(const Vector4<T>& v) const
    {
        return Vector4<T>(x / v.x, y / v.y, z / v.z, w / v.w);
    }
    
    /// Divides all components of this vector by the scalar s.
    Vector4<T>& operator/=(T s)
    {
        x /= s;
        y /= s;
        z /= s;
        w /= s;
        return *this;
    }
    
    /// Component-wise division of this vector and v.
    Vector4<T>& operator/=(const Vector4<T>& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
        return *this;
    }
    
    /// Returns the zero vector (0,0,0,0).
    static const Vector4<T>& zero()
    {
        static Vector4<T> s_zero;
        return s_zero;
    }
    
    /// Returns the one vector (1,1,1,1).
    static const Vector4<T>& one()
    {
        static Vector4<T> s_one(1);
        return s_one;
    }
    
    /// Returns the x-axis vector (1,0,0,0).
    static const Vector4<T>& xAxis()
    {
        static Vector4<T> s_x(1,0,0,0);
        return s_x;
    }
    
    /// Returns the y-axis vector (0,1,0,0).
    static const Vector4<T>& yAxis()
    {
        static Vector4<T> s_y(0,1,0,0);
        return s_y;
    }
    
    /// Returns the z-axis vector (0,0,1,0).
    static const Vector4<T>& zAxis()
    {
        static Vector4<T> s_z(0,0,1,0);
        return s_z;
    }
    
    /// Returns the w-axis vector (0,0,0,1).
    static const Vector4<T>& wAxis()
    {
        static Vector4<T> s_w(0,0,0,1);
        return s_w;
    }
    
    /// Prints the vector to an output stream.
    friend std::ostream& operator<<(std::ostream& os, const Vector4<T>& v)
    {
        os << std::fixed;
        os << std::setprecision(3);
        os << "(";
        os << std::setw(10) << v.x << " ";
        os << std::setw(10) << v.y << " ";
        os << std::setw(10) << v.z << " ";
        os << std::setw(10) << v.w << ")";
        return os;
    }
};

typedef Vector4<float> Vec4;
typedef Vector4<double> Vec4d;

#endif
