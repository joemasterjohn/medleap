#ifndef CGL_MATRIX2_H_
#define CGL_MATRIX2_H_

#include <iostream>
#include <iomanip>
#include "Math.h"

namespace cgl
{
  template <typename T> class Matrix2;
  typedef Matrix2<float> Mat2;
  typedef Matrix2<double> Mat2d;
  
  /// 2x2 matrix. The values are ordered by column first:
  ///
  /// M = | a c |
  ///     | b d |
  template <typename T> class Matrix2
  {
    T m[4];

  public:

    /// Constructs a new identity matrix.
    Matrix2<T>()
    {
      m[0] = 1;
      m[1] = 0;
      m[2] = 0;
      m[3] = 1;
    }

    /// Constructs a new matrix given the column vectors (a,b) and (c,d).
    Matrix2<T>(T a, T b, T c, T d)
    {
      m[0] = a;
      m[1] = b;
      m[2] = c;
      m[3] = d;
    }

    /// Constructs a new matrix given the column vectors x and y.
    Matrix2<T>(const Vector2<T>& x, const Vector2<T>& y)
    {
      m[0] = x.x;
      m[1] = x.y;
      m[2] = y.x;
      m[2] = y.y;
    }

    /// Constructs a new matrix by copying the values in an array.
    Matrix2<T>(const T* values)
    {
      for (int i = 0; i < 4; ++i)
        m[i] = values[i];
    }

    /// Constructs a copy of the matrix m.
    Matrix2<T>(const Matrix2<T>& mat)
    {
      for (int i = 0; i < 4; ++i)
        m[i] = mat.m[i];
    }

    /// Returns a copy of the row vector at index i.
    Vector2<T> row(int i) const
    {
      return Vector2<T>(m[i], m[i + 2]);
    }

    /// Returns a copy of the column vector at index i.
    Vector2<T> col(int i) const
    {
      int j = i * 2;
      return Vector2<T>(m[j], m[j + 1]);
    }

    /// Returns the determinant of the matrix.
    T determinant() const
    {
      return det(m[0], m[1], m[2], m[3]);
    }

    /// Sum of the diagonal components.
    T trace() const
    {
      return m[0] + m[3];
    }

    /// Computes the inverse of this matrix; if not possible, returns identity.
    Matrix2<T> inverse() const
    {
      T det = determinant();
      if (det == 0) return Matrix2<T>();
      return Matrix2<T>(m[3] / det, -m[1] / det, -m[2] / det, m[0] / det);
    }

    /// Returns the transpose of this matrix.
    Matrix2<T> transpose() const
    {
      return Matrix2<T>(m[0], m[2], m[1], m[3]);
    }

    /// Converts the matrix to an array.
    operator const T*() const
    {
      return m;
    }

    /// Multiplies this matrix with a column vector v.
    Vector2<T> operator*(const Vector2<T>& v) const
    {
      return Vector2<T>(row(0).dot(v), row(1).dot(v));
    }

    /// Multiplies this matrix with another matrix B.
    Matrix2<T> operator*(const Matrix2<T>& B) const
    {
      return Matrix2<T>((*this) * B.col(0), (*this) * B.col(1));
    }

    /// Multiplies the matrix by a scalar s.
    Matrix2<T> operator*(T s) const
    {
      return Matrix2<T>(m[0] * s, m[1] * s, m[2] * s, m[3] * s);
    }

    /// Adds the matrix B to this matrix.
    Matrix2<T> operator+(const Matrix2<T>& B) const
    {
      return Matrix2<T>(m[0] + B.m[0], m[1] + B.m[1], m[2] + B.m[2], m[3] + B.m[3]);
    }

    /// Prints the matrix to an output stream.
    friend std::ostream& operator<<(std::ostream& os, const Matrix2<T>& M)
    {
      return os << M.row(0) << std::endl << M.row(1) << std::endl;
    }

    /// Creates a rotation matrix.
    static Matrix2<T> rotation(T radians)
    {
      T c = std::cos(radians);
      T s = std::sin(radians);
      return Matrix2<T>(c, s, -s, c);
    }

    /// Creates a scale matrix.
    static Matrix2<T> scale(T x, T y)
    {
      return Matrix2<T>(x, 0, 0, y);
    }

    /// Computes determinant of matrix with columns (a,b) and (c,d).
    static T det(T a, T b, T c, T d)
    {
      return a * d - b * c;
    }
  };

} // namespace cgl

#endif
