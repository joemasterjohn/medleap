#ifndef __CGL_GEOM_BOX_H__
#define __CGL_GEOM_BOX_H__

#include "gl/glew.h"
#include "gl/math/Vector3.h"
#include "Geometry.h"

namespace gl
{
    class Box : public Geometry<Vec3, GLuint>
    {
    public:
		Box(GLfloat edgeLength, unsigned segments = 1);
		Box(GLfloat width, GLfloat height, GLfloat depth, unsigned widthSegments = 1, unsigned heightSegments = 1, unsigned depthSegments = 1);
    };
}

#endif // __CGL_GEOM_BOX_H__