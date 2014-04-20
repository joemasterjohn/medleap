#ifndef CGL_MATH_H_
#define CGL_MATH_H_

#include <cmath>

namespace gl
{
	static const double pi = 3.14159265359;
	static const double two_pi = pi * 2.0;
	static const double pi_over_2 = pi / 2.0;
	static const double pi_over_3 = pi / 3.0;
	static const double pi_over_4 = pi / 4.0;
	static const double pi_over_6 = pi / 6.0;

	/** Calculates the screen space radius of a sphere:
	  * fovY = vertical field of view used in the projection (radians)
	  * d = distance between camera center/eye to center of sphere
	  * r = radius of sphere in world space
	  */
	double projectedRadius(double fovY, double d, double r);
}


#endif // CGL_MATH_H_