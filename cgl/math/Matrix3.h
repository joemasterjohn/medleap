#ifndef CGL_MATRIX3_H_
#define CGL_MATRIX3_H_

#include <iostream>
#include <iomanip>
#include "Vector3.h"
#include "Matrix2.h"

namespace cgl
{
    template <typename T> class Vector3;
    
    /// A 3x3 matrix. The values are stored by column first:
    ///
    /// M = | a d g |
    ///     | b e h |
    ///     | c f i |
    template <typename T> class Matrix3
    {
        T m[9];
        
    public:
        
        /// Constructs a new identity matrix.
        Matrix3<T>()
        {
            m[0] = 1;
            m[1] = 0;
            m[2] = 0;
            
            m[3] = 0;
            m[4] = 1;
            m[5] = 0;
            
            m[6] = 0;
            m[7] = 0;
            m[8] = 1;
        }
        
        /// Constructs a new matrix given the column vectors (a,b,c), (d,e,f),
        /// and (g,h,i).
        Matrix3<T>(T a, T b, T c, T d, T e, T f, T g, T h, T i)
        {
            m[0] = a;
            m[1] = b;
            m[2] = c;
            
            m[3] = d;
            m[4] = e;
            m[5] = f;
            
            m[6] = g;
            m[7] = h;
            m[8] = i;
        }
        
        /// Constructs a new matrix given the column vectors x, y, and z.
        Matrix3<T>(const Vector3<T>& x, const Vector3<T>& y, const Vector3<T>& z)
        {
            m[0] = x.x;
            m[1] = x.y;
            m[2] = x.z;
            
            m[3] = y.x;
            m[4] = y.y;
            m[5] = y.z;
            
            m[6] = z.x;
            m[7] = z.y;
            m[8] = z.z;
        }
        
        /// Constructs a new matrix by copying the values in an array.
        Matrix3<T>(const T* values)
        {
            for (int i = 0; i < 9; ++i)
                m[i] = values[i];
        }
        
        /// Constructs a copy of the matrix B.
        Matrix3<T>(const Matrix3<T>& B)
        {
            for (int i = 0; i < 9; ++i)
                m[i] = B.m[i];
        }
        
        /// Returns a copy of the row vector at index i.
        Vector3<T> row(int i) const
        {
            return Vector3<T>(m[i], m[i + 3], m[i + 6]);
        }
        
        /// Returns a copy of the column vector at index i.
        Vector3<T> col(int i) const
        {
            int j = i * 3;
            return Vector3<T>(m[j], m[j + 1], m[j + 2]);
        }
        
        /// Returns the determinant of the matrix.
        T determinant() const
        {
            return m[0] * Matrix2<T>::det(m[4], m[5], m[7], m[8]) -
            m[3] * Matrix2<T>::det(m[1], m[2], m[7], m[8]) +
            m[6] * Matrix2<T>::det(m[1], m[2], m[4], m[5]);
        }
        
        /// Computes the sum of the diagonal.
        T trace() const
        {
            return m[0] + m[4] + m[8];
        }
        
        /// Computes the inverse of this matrix; if not possible, returns identity.
        Matrix3<T> inverse() const
        {
            T det = determinant();
            if (det == 0) return Matrix3<T>();
            
            T a = Matrix2<T>::det(m[4], m[5], m[7], m[8]) / det;
            T b = Matrix2<T>::det(m[7], m[8], m[1], m[2]) / det;
            T c = Matrix2<T>::det(m[1], m[2], m[4], m[5]) / det;
            T d = Matrix2<T>::det(m[6], m[8], m[3], m[5]) / det;
            T e = Matrix2<T>::det(m[0], m[2], m[6], m[8]) / det;
            T f = Matrix2<T>::det(m[3], m[5], m[0], m[2]) / det;
            T g = Matrix2<T>::det(m[3], m[4], m[6], m[7]) / det;
            T h = Matrix2<T>::det(m[6], m[7], m[0], m[1]) / det;
            T i = Matrix2<T>::det(m[0], m[1], m[3], m[4]) / det;
            
            return Matrix3<T>(a, b, c, d, e, f, g, h, i);
        }
        
        /// Computes the transpose of this matrix.
        Matrix3<T> transpose() const
        {
            return Matrix3<T>(m[0], m[3], m[6], m[1], m[4], m[7], m[2], m[5], m[8]);
        }
        
        /// Converts the matrix to an array.
        operator const T*() const
        {
            return m;
        }
        
        /// Multiplies this matrix with a column vector v.
        Vector3<T> operator*(const Vector3<T>& v) const
        {
            return Vector3<T>(row(0).dot(v), row(1).dot(v), row(2).dot(v));
        }
        
        /// Multiplies this matrix with another matrix B.
        Matrix3<T> operator*(const Matrix3<T>& B) const
        {
            return Matrix3<T>((*this) * B.col(0), (*this) * B.col(1), (*this) * B.col(2));
        }
        
        /// Multiplies this matrix by a scalar.
        Matrix3<T> operator*(T s) const
        {
            return Matrix3<T>(m[0] * s, m[1] * s, m[2] * s,
                              m[3] * s, m[4] * s, m[5] * s,
                              m[6] * s, m[7] * s, m[8] * s);
        }
        
        /// Adds the matrix B to this matrix.
        Matrix3<T> operator+(const Matrix3<T>& B) const
        {
            return Matrix3<T>(m[0] + B.m[0], m[1] + B.m[1], m[2] + B.m[2],
                              m[3] + B.m[3], m[4] + B.m[4], m[5] + B.m[5],
                              m[6] + B.m[6], m[7] + B.m[7], m[8] + B.m[8]);
        }
        
        /// Prints the matrix to an output stream.
        friend std::ostream& operator<<(std::ostream& os, const Matrix3<T>& M)
        {
            os << M.row(0) << std::endl;
            os << M.row(1) << std::endl;
            os << M.row(2) << std::endl;
            return os;
        }
        
        /// Creates a translation matrix (use in 2D with homogeneous coordinates).
        static Matrix3<T> translation(T x, T y)
        {
            return Matrix3<T>(1, 0, 0, 0, 1, 0, x, y, 1);
        }
        
        /// Creates a scale matrix.
        static Matrix3<T> scale(T x, T y, T z)
        {
            return Matrix3<T>(x, 0, 0, 0, y, 0, 0, 0, z);
        }
        
        /// Creates a rotation matrix around the X axis.
        static Matrix3<T> rotationX(T radians)
        {
            T c = std::cos(radians);
            T s = std::sin(radians);
            return Matrix3<T>(1, 0, 0, 0, c, s, 0, -s, c);
        }
        
        /// Creates a rotation matrix around the Y axis.
        static Matrix3<T> rotationY(T radians)
        {
            T c = std::cos(radians);
            T s = std::sin(radians);
            return Matrix3<T>(c, 0, -s, 0, 1, 0, s, 0, c);
        }
        
        /// Creates a rotation matrix around the Z axis.
        static Matrix3<T> rotationZ(T radians)
        {
            T c = std::cos(radians);
            T s = std::sin(radians);
            return Matrix3<T>(c, s, 0, -s, c, 0, 0, 0, 1);
        }
    };
    
    typedef Matrix3<float> Mat3;
    typedef Matrix3<double> Mat3d;
} // namespace cgl

#endif
