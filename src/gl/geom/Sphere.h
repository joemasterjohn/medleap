#ifndef __CGL_GEOM_SPHERE_H__
#define __CGL_GEOM_SPHERE_H__

#include "gl/glew.h"
#include "Geometry.h"

namespace gl
{
	class Sphere : public Geometry<Vec3, GLuint>
    {
    public:
		Sphere(GLfloat radius, unsigned segments = 1);
    };
}

#endif // __CGL_GEOM_SPHERE_H__