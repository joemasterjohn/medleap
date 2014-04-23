#ifndef __MEDLEA_UTIL_INTERVAL_H__
#define __MEDLEA_UTIL_INTERVAL_H__

#include <limits>
#include "gl/math/Math.h"

/** Interval of normalized values. Guarantees center, left, and right values will always be in [0,1]. */
class Interval
{
public:
	/** Creates a new interval that defaults to the entire range [0,1] */
	Interval() : center_(0.5f), width_(1.0f), left_(0.0f), right_(1.0f) {}

	/** Creates a new interval [center-width/2, center+width/2]*/
	Interval(float center, float width) {
		this->center(center);
		this->width(width);
	}

	/** Center value of influence area in [0,1] */
	float center() const { return center_; }

	/** Minimum value of influence in [0,1] */
	float left() const { return left_; }

	/** Maximum value of influence in [0,1] */
	float right() const { return right_; }

	/** Width of influence in [0,1] */
	float width() const { return width_; }

	/** Sets the center value of the area of influence in [0, 1]. */
	void center(float value) {
		center_ = gl::clamp(value, 0.0f, 1.0f);
		left_ = gl::clamp(center_ - (width_ / 2.0f), 0.0f, 1.0f);
		right_ = gl::clamp(center_ + (width_ / 2.0f), 0.0f, 1.0f);
	}

	/** Sets the width of influence in [0, 1]. */
	void width(float value) {
		// clamped to max of 2.0 in case center is at 0.0 or 1.0
		// and I want to include all values regardless of center
		width_ = gl::clamp(value, 0.0f, 2.0f);
		left_ = gl::clamp(center_ - (width_ / 2.0f), 0.0f, 1.0f);
		right_ = gl::clamp(center_ + (width_ / 2.0f), 0.0f, 1.0f);
	}

	/** Sets the area of influence bounded by left/right in [0,1]. If left > right, the values will be swapped. */
	void width(float left, float right) {
		if (left > right) {
			left_ = gl::clamp(right, 0.0f, 1.0f);
			right_ = gl::clamp(left, 0.0f, 1.0f);
		}
		else {
			left_ = gl::clamp(left, 0.0f, 1.0f);
			right_ = gl::clamp(right, 0.0f, 1.0f);
		}

		width_ = std::max(0.0f, right_ - left_);
		center_ = left_ + width_ / 2.0f;
	}

	/** Center value of influence area in [Tmin, Tmax] */
	template <typename T> T center() const { return gl::unnormalize<T>(center_); };

	/** Minimum value of influence in [Tmin, Tmax] */
	template <typename T> T left() const { return gl::unnormalize<T>(left_); }

	/** Maximum value of influence in [Tmin, Tmax] */
	template <typename T> T right() const { return gl::unnormalize<T>(right_); }

	/** Width of influence in [Tmin, Tmax] */
	template <typename T> T width() const { return gl::unnormalize<T>(width_); }

	/** Sets the center value of the area of influence in [Tmin, Tmax]. */
	template <typename T> void center(T value) { center(gl::normalize<T>(value)); }

	/** Sets the width of influence in [Tmin, Tmax]. */
	template <typename T> void width(T value) { width(gl::normalize<T>(value)); }

	/** Sets the area of influence bounded by left/right in [Tmin, Tmax]. If left > right, the values will be swapped. */
	template <typename T> void width(T left, T right) { width(gl::normalize<T>(left), gl::normalize<T>(right)); }

private:
	float center_;
	float left_;
	float right_;
	float width_;
};

#endif // __MEDLEA_UTIL_INTERVAL_H__