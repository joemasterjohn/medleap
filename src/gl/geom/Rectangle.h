#ifndef __GL_GEOM_RECTANGLE_H__
#define __GL_GEOM_RECTANGLE_H__

#include "gl/math/Vector2.h"

namespace gl
{
	template <typename T> class Rectangle
	{
	public:
		T x, y, width, height;

		Rectangle() : x(0), y(0), width(1), height(1) {}
		Rectangle(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {}

		T left() const {
			return x;
		}

		T right() const {
			return x + width;
		}

		T bottom() const {
			return y;
		}

		T top() const {
			return y + height;
		}

		Vec2 center() const {
			return{ width / 2.0f + x, height / 2.0f + y };
		}

		bool operator==(const Rectangle<T>& v) const
		{
			return (x == v.x && y == v.y && width == v.width && height == v.height);
		}

		bool operator!=(const Rectangle<T>& v) const
		{
			return (x != v.x || y != v.y || width != v.width || height != v.height);
		}

		bool contains(T x, T y) const
		{
			return (x >= this->x &&
				x <= this->x + width &&
				y >= this->y &&
				y <= this->y + height);
		}

		float aspect() const {
			return static_cast<double>(width) / height;
		}
	};
}

#endif // __GL_GEOM_RECTANGLE_H__