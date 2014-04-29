#ifndef __GL_GEOM_BOX_H__
#define __GL_GEOM_BOX_H__

#include "gl/glew.h"
#include "gl/util/Geometry.h"

namespace gl
{
    class Box
    {
    public:
		Box(float size);
		Box(float width, float height, float length);

		Geometry triangles(unsigned x_segments, unsigned y_segments, unsigned z_segments);
    
	private:
		Vec3 size_;
	};
}

#endif // __CGL_GEOM_BOX_H__