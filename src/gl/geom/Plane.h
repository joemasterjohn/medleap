#ifndef __CGL_GEOM_PLANE_H__
#define __CGL_GEOM_PLANE_H__

#include "gl/glew.h"
#include "gl/math/Vector3.h"
#include "Geometry.h"
#include <vector>

namespace gl
{
    class Plane : public Geometry<Vec3, GLuint>
    {
	public:
		Plane(float width, float height, unsigned widthSegments = 1, unsigned heightSegments = 1);
    };
}

#endif // __CGL_GEOM_PLANE_H__