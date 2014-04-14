#include "Math.h"

double gl::projectedRadius(double fovY, double d, double r) {
	// http://stackoverflow.com/questions/21648630/radius-of-projected-sphere-in-screen-space?lq=1
	return 1.0 / std::tan(fovY / 2.0) * r / std::sqrt(d * d - r * r);
}