#ifndef CGL_VECTOR2_H_
#define CGL_VECTOR2_H_

#include <iostream>
#include <iomanip>
#include <cmath>

namespace cgl
{
    /// 2D Vector with X and Y components.
    template <typename T> class Vector2
    {
    public:
        
        /// The X component.
        T x;
        
        /// The Y component.
        T y;
        
        /// Constructs a 2D vector with both components set to s (default 0).
        Vector2<T>(T s = 0) : x(s), y(s) {}
        
        /// Constructs a 2D vector with X and Y components set to given values.
        Vector2<T>(T x, T y) : x(x), y(y) {}
        
        /// Constructs a 2D vector by copying the first two values from v.
        Vector2<T>(const Vector3<T>& v) : x(v.x), y(v.y) {}
        
        /// Constructs a 2D vector from an array.
        Vector2<T>(const T a[2]) : x(a[0]), y(a[1]) {}
        
        /// Returns the number of components in the vector (2).
        unsigned size() const
        {
            return 2;
        }
        
        /// Computes the length/magnitude squared of the vector.
        T lengthSquared() const
        {
            return x * x + y * y;
        }
        
        /// Computes the length/magnitude of the vector.
        T length() const
        {
            return std::sqrt(lengthSquared());
        }
        
        /// Computes the angle in [-pi, pi].
        T angle() const
        {
            return std::atan2(y, x);
        }
        
        /// Computes the angle in [0, 2pi].
        T anglePositive() const
        {
            T rads = angle();
            return rads < 0 ? 2 * M_PI + rads : rads;
        }
        
        /// Computes the angle from this vector to v (always positive).
        double angleTo(const Vector2<T>& v) const
        {
            double a1 = anglePositive();
            double a2 = v.anglePositive();
            if (a2 < a1)
                a2 += M_PI * 2.0;
            return a2 - a1;
        }
        
        /// Compute the dot product.
        T dot(const Vector2<T>& v) const
        {
            return x * v.x + y * v.y;
        }
        
        /// Computes the 2D cross product.
        T cross(const Vector2<T>& v) const
        {
            return x * v.y - y * v.x;
        }
        
        /// Returns the reflection of this vector on a surface with normal n.
        Vector2<T> reflect(const Vector2<T>& n) const
        {
            return (*this) - n * n.dot(*this) * 2;
        }
        
        /// Returns a copy of this vector normalized.
        Vector2<T> normal() const
        {
            return (*this) / length();
        }
        
        /// Rotates this vector by radians around the origin.
        Vector2<T>& rotate(T radians)
        {
            T c = std::cos(radians);
            T s = std::sin(radians);
            T x1 = c * x - s * y;
            T y1 = s * x + c * y;
            x = x1;
            y = y1;
            return *this;
        }
        
        /// Rotates this vector 90 degrees.
        Vector2<T>& rotate90()
        {
            T temp = x;
            x = -y;
            y = temp;
            return *this;
        }
        
        /// Rotates this vector 180 degrees.
        Vector2<T>& rotate180()
        {
            x = -x;
            y = -y;
            return *this;
        }
        
        /// Rotates this vector 270 degrees.
        Vector2<T>& rotate270()
        {
            T temp = x;
            x = y;
            y = -temp;
            return *this;
        }
        
        /// Scales the components to ensure the vector has unit length.
        Vector2<T>& normalize()
        {
            return (*this) /= length();
        }
        
        /// Treats the vector as an array of values.
        operator T*()
        {
            return &x;
        }
        
        /// Treats the vector as an array of values.
        operator const T*() const
        {
            return &x;
        }
        
        /// Returns a reference to x (index == 0) or y (index == 1).
        T& operator[](int index)
        {
            return *(&x + index);
        }
        
        /// Returns a copy of x (index == 0) or y (index == 1).
        T operator[](int index) const
        {
            return *(&x + index);
        }
        
        /// Returns true if the two vectors have the exact same component values.
        bool operator==(const Vector2<T>& v) const {
            return x == v.x && y == v.y;
        }
        
        /// Returns true if the two vectors have at least one component that differs.
        bool operator!=(const Vector2<T>& v) const
        {
            return x != v.x || y != v.y;
        }
        
        /// Negates all components of this vector.
        Vector2<T> operator-() const
        {
            return Vector2<T>(-x, -y);
        }
        
        /// Adds the scalar s to all components of this vector.
        Vector2<T> operator+(T s) const
        {
            return Vector2<T>(this->x + s, this->y + s);
        }
        
        /// Component-wise addition of this vector and v.
        Vector2<T> operator+(const Vector2<T>& v) const
        {
            return Vector2<T>(x + v.x, y + v.y);
        }
        
        /// Adds the scalar s to all components of the vector.
        Vector2<T>& operator+=(T s)
        {
            x += s;
            y += s;
            return *this;
        }
        
        /// Component-wise addition of this vector and v.
        Vector2<T>& operator+=(const Vector2<T>& v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }
        
        /// Subtracts the scalar s from all components of this vector.
        Vector2<T> operator-(T s) const
        {
            return Vector2<T>(this->x - s, this->y - s);
        }
        
        /// Component-wise subtraction of this vector and v.
        Vector2<T> operator-(const Vector2<T>& v) const
        {
            return Vector2<T>(x - v.x, y - v.y);
        }
        
        /// Subtracts the scalar s from all components of the vector.
        Vector2<T>& operator-=(T s)
        {
            x -= s;
            y -= s;
            return *this;
        }
        
        /// Component-wise subtraction of this vector and v.
        Vector2<T>& operator-=(const Vector2<T>& v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }
        
        /// Multiplies the scalar s with all components of this vector.
        Vector2<T> operator*(T s) const
        {
            return Vector2<T>(this->x * s, this->y * s);
        }
        
        /// Component-wise multiplication of this vector and v.
        Vector2<T> operator*(const Vector2<T>& v) const
        {
            return Vector2<T>(x * v.x, y * v.y);
        }
        
        /// Multiplies the scalar s with all components of the vector.
        Vector2<T>& operator*=(T s)
        {
            x *= s;
            y *= s;
            return *this;
        }
        
        /// Component-wise multiplication of this vector and v.
        Vector2<T>& operator*=(const Vector2<T>& v)
        {
            x *= v.x;
            y *= v.y;
            return *this;
        }
        
        /// Divides all components of this vector by the scalar s.
        Vector2<T> operator/(T s) const
        {
            return Vector2<T>(this->x / s, this->y / s);
        }
        
        /// Component-wise division of this vector and v.
        Vector2<T> operator/(const Vector2<T>& v) const
        {
            return Vector2<T>(x / v.x, y / v.y);
        }
        
        /// Divides all components of this vector by the scalar s.
        Vector2<T>& operator/=(T s)
        { 
            x /= s;
            y /= s;
            return *this;
        } 
        
        /// Component-wise division of this vector and v.
        Vector2<T>& operator/=(const Vector2<T>& v)
        { 
            x /= v.x;
            y /= v.y;
            return *this;
        }
        
        /// Returns the zero vector (0,0).
        static const Vector2<T>& zero()
        { 
            static Vector2<T> s_zero;
            return s_zero;
        }
        
        /// Returns the one vector (1,1).
        static const Vector2<T>& one() 
        {
            static Vector2<T> s_one(1);
            return s_one; 
        }
        
        /// Returns the x-axis vector (1,0).
        static const Vector2<T>& xAxis()
        {
            static Vector2<T> s_x(1,0); 
            return s_x;
        }
        
        /// Returns the y-axis vector (0,1).
        static const Vector2<T>& yAxis()
        { 
            static Vector2<T> s_y(0,1);
            return s_y;
        }
        
        /// Prints the vector to an output stream.
        friend std::ostream& operator<<(std::ostream& os, const Vector2<T>& v)
        {
            os << std::fixed;
            os << std::setprecision(3);
            os << "(" << std::setw(10) << v.x << " " << std::setw(10) << v.y << ")";
            return os;
        }
    };
    
    typedef Vector2<float> Vec2;
    typedef Vector2<double> Vec2d;
    
} // namespace cgl

#endif
