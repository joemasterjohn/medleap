#ifndef __GL_MATH_MATRIX4_H__
#define __GL_MATH_MATRIX4_H__

#include "Math.h"

namespace gl
{
	/// A 4x4 matrix. The values are stored by column first:
	///
	/// M = | a e i m |
	///     | b f j n |
	///     | c g k o |
	///     | d h l p |
	template <typename T> class Matrix4
	{
		T m[16];

	public:

		/// Constructs a new identity matrix.
		Matrix4<T>()
		{
			for (int i = 0; i < 16; ++i)
				m[i] = 0;
			m[0] = m[5] = m[10] = m[15] = 1;
		}

		/// Constructs a new matrix given the column vectors (a,b,c,d), (e,f,g,h),
		/// (i, j, k, l), and (m, n, o, p).
		Matrix4<T>(T a, T b, T c, T d,
			T e, T f, T g, T h,
			T i, T j, T k, T l,
			T m, T n, T o, T p)
		{
			this->m[0] = a;
			this->m[1] = b;
			this->m[2] = c;
			this->m[3] = d;

			this->m[4] = e;
			this->m[5] = f;
			this->m[6] = g;
			this->m[7] = h;

			this->m[8] = i;
			this->m[9] = j;
			this->m[10] = k;
			this->m[11] = l;

			this->m[12] = m;
			this->m[13] = n;
			this->m[14] = o;
			this->m[15] = p;
		}

		/// Constructs a new matrix given the column vectors x, y, z, and w.
		Matrix4<T>(const Vector4<T>& x, const Vector4<T>& y, const Vector4<T>& z,
			const Vector4<T>& w)
		{
			m[0] = x.x;
			m[1] = x.y;
			m[2] = x.z;
			m[3] = x.w;

			m[4] = y.x;
			m[5] = y.y;
			m[6] = y.z;
			m[7] = y.w;

			m[8] = z.x;
			m[9] = z.y;
			m[10] = z.z;
			m[11] = z.w;

			m[12] = w.x;
			m[13] = w.y;
			m[14] = w.z;
			m[15] = w.w;
		}

		/// Constructs a new matrix by copying the values in an array.
		Matrix4<T>(const T* a)
		{
			for (int i = 0; i < 16; ++i)
				m[i] = a[i];
		}

		/// Constructs a copy of the matrix B.
		Matrix4<T>(const Matrix4<T>& B)
		{
			for (int i = 0; i < 16; ++i)
				m[i] = B.m[i];
		}

		/// Creates a 4x4 homogeneous matrix from a 3x3 matrix
		Matrix4<T>(const Matrix3<T>& M)
		{
			m[0] = M.col(0).x;
			m[1] = M.col(0).y;
			m[2] = M.col(0).z;
			m[3] = 0;

			m[4] = M.col(1).x;
			m[5] = M.col(1).y;
			m[6] = M.col(1).z;
			m[7] = 0;

			m[8] = M.col(2).x;
			m[9] = M.col(2).y;
			m[10] = M.col(2).z;
			m[11] = 0;

			m[12] = 0;
			m[13] = 0;
			m[14] = 0;
			m[15] = 1;
		}

		/// Returns a copy of the row vector at index i.
		Vector4<T> row(int i) const
		{
			return Vector4<T>(m[i], m[i + 4], m[i + 8], m[i + 12]);
		}

		/// Returns a copy of the column vector at index i.
		Vector4<T> col(int i) const
		{
			int j = i * 4;
			return Vector4<T>(m[j], m[j + 1], m[j + 2], m[j + 3]);
		}

		/// Computes the sum of the diagonal.
		T trace() const
		{
			return m[0] + m[5] + m[10] + m[15];
		}

		/** Returns a matrix containing only the rotation/scale components */
		Matrix4<T> rotScale() const
		{
			return Matrix4(col(0), col(1), col(2), Vec4(0, 0, 0, 1));
		}

		/// Computes the inverse of the matrix; returns identity if none exists.
		Matrix4<T> inverse() const
		{
			// adapted from MESA implementation of gluInvertMatrix
			T invOut[16];
			T inv[16], det;
			int i;

			inv[0] = m[5] * m[10] * m[15] -
				m[5] * m[11] * m[14] -
				m[9] * m[6] * m[15] +
				m[9] * m[7] * m[14] +
				m[13] * m[6] * m[11] -
				m[13] * m[7] * m[10];

			inv[4] = -m[4] * m[10] * m[15] +
				m[4] * m[11] * m[14] +
				m[8] * m[6] * m[15] -
				m[8] * m[7] * m[14] -
				m[12] * m[6] * m[11] +
				m[12] * m[7] * m[10];

			inv[8] = m[4] * m[9] * m[15] -
				m[4] * m[11] * m[13] -
				m[8] * m[5] * m[15] +
				m[8] * m[7] * m[13] +
				m[12] * m[5] * m[11] -
				m[12] * m[7] * m[9];

			inv[12] = -m[4] * m[9] * m[14] +
				m[4] * m[10] * m[13] +
				m[8] * m[5] * m[14] -
				m[8] * m[6] * m[13] -
				m[12] * m[5] * m[10] +
				m[12] * m[6] * m[9];

			inv[1] = -m[1] * m[10] * m[15] +
				m[1] * m[11] * m[14] +
				m[9] * m[2] * m[15] -
				m[9] * m[3] * m[14] -
				m[13] * m[2] * m[11] +
				m[13] * m[3] * m[10];

			inv[5] = m[0] * m[10] * m[15] -
				m[0] * m[11] * m[14] -
				m[8] * m[2] * m[15] +
				m[8] * m[3] * m[14] +
				m[12] * m[2] * m[11] -
				m[12] * m[3] * m[10];

			inv[9] = -m[0] * m[9] * m[15] +
				m[0] * m[11] * m[13] +
				m[8] * m[1] * m[15] -
				m[8] * m[3] * m[13] -
				m[12] * m[1] * m[11] +
				m[12] * m[3] * m[9];

			inv[13] = m[0] * m[9] * m[14] -
				m[0] * m[10] * m[13] -
				m[8] * m[1] * m[14] +
				m[8] * m[2] * m[13] +
				m[12] * m[1] * m[10] -
				m[12] * m[2] * m[9];

			inv[2] = m[1] * m[6] * m[15] -
				m[1] * m[7] * m[14] -
				m[5] * m[2] * m[15] +
				m[5] * m[3] * m[14] +
				m[13] * m[2] * m[7] -
				m[13] * m[3] * m[6];

			inv[6] = -m[0] * m[6] * m[15] +
				m[0] * m[7] * m[14] +
				m[4] * m[2] * m[15] -
				m[4] * m[3] * m[14] -
				m[12] * m[2] * m[7] +
				m[12] * m[3] * m[6];

			inv[10] = m[0] * m[5] * m[15] -
				m[0] * m[7] * m[13] -
				m[4] * m[1] * m[15] +
				m[4] * m[3] * m[13] +
				m[12] * m[1] * m[7] -
				m[12] * m[3] * m[5];

			inv[14] = -m[0] * m[5] * m[14] +
				m[0] * m[6] * m[13] +
				m[4] * m[1] * m[14] -
				m[4] * m[2] * m[13] -
				m[12] * m[1] * m[6] +
				m[12] * m[2] * m[5];

			inv[3] = -m[1] * m[6] * m[11] +
				m[1] * m[7] * m[10] +
				m[5] * m[2] * m[11] -
				m[5] * m[3] * m[10] -
				m[9] * m[2] * m[7] +
				m[9] * m[3] * m[6];

			inv[7] = m[0] * m[6] * m[11] -
				m[0] * m[7] * m[10] -
				m[4] * m[2] * m[11] +
				m[4] * m[3] * m[10] +
				m[8] * m[2] * m[7] -
				m[8] * m[3] * m[6];

			inv[11] = -m[0] * m[5] * m[11] +
				m[0] * m[7] * m[9] +
				m[4] * m[1] * m[11] -
				m[4] * m[3] * m[9] -
				m[8] * m[1] * m[7] +
				m[8] * m[3] * m[5];

			inv[15] = m[0] * m[5] * m[10] -
				m[0] * m[6] * m[9] -
				m[4] * m[1] * m[10] +
				m[4] * m[2] * m[9] +
				m[8] * m[1] * m[6] -
				m[8] * m[2] * m[5];

			det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

			if (det == 0)
				return Matrix4<T>();

			det = static_cast<T>(1.0 / det);

			for (i = 0; i < 16; i++)
				invOut[i] = inv[i] * det;

			return Matrix4<T>(invOut);
		}

		/// Computes the transpose of the matrix.
		Matrix4<T> transpose() const
		{
			return Matrix4<T>(m[0], m[4], m[8], m[12],
				m[1], m[5], m[9], m[13],
				m[2], m[6], m[10], m[14],
				m[3], m[7], m[11], m[15]);
		}

		/// Converts the matrix to an array.
		operator const T*() const
		{
			return m;
		}

		/// Multiplies this matrix with a column vector v.
		Vector4<T> operator*(const Vector4<T>& v) const
		{
			return Vector4<T>(row(0).dot(v), row(1).dot(v), row(2).dot(v), row(3).dot(v));
		}

		/// Multiplies this matrix with another matrix B.
		Matrix4<T> operator*(const Matrix4<T>& B) const
		{
			return Matrix4<T>((*this) * B.col(0),
				(*this) * B.col(1),
				(*this) * B.col(2),
				(*this) * B.col(3));
		}

		/// Multiplies this matrix by a scalar.
		Matrix4<T> operator*(T s) const
		{
			return Matrix4<T>(m[0] * s, m[1] * s, m[2] * s, m[3] * s,
				m[4] * s, m[5] * s, m[6] * s, m[7] * s,
				m[8] * s, m[9] * s, m[10] * s, m[11] * s,
				m[12] * s, m[13] * s, m[14] * s, m[15] * s);
		}

		/// Adds the matrix B to this matrix.
		Matrix4<T> operator+(const Matrix4<T>& B) const
		{
			return Matrix4<T>(col(0) + B.col(0),
				col(1) + B.col(1),
				col(2) + B.col(2),
				col(3) + B.col(3));
		}

		/// Prints the matrix to an output stream.
		friend std::ostream& operator<<(std::ostream& os, const Matrix4<T>& M)
		{
			os << M.row(0) << std::endl;
			os << M.row(1) << std::endl;
			os << M.row(2) << std::endl;
			os << M.row(3) << std::endl;
			return os;
		}
	};
}

#endif // __GL_MATH_MATRIX4_H__