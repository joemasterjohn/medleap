#ifndef CGL_MATH_H_
#define CGL_MATH_H_

#include <cmath>

namespace gl
{
	static const double PI = 3.14159265359;
	static const double PI2 = 6.28318530718;

	/** Calculates the screen space radius of a sphere:
	  * fovY = vertical field of view used in the projection (radians)
	  * d = distance between camera center/eye to center of sphere
	  * r = radius of sphere in world space
	  */
	double projectedRadius(double fovY, double d, double r);
}


#endif // CGL_MATH_H_