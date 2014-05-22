#ifndef __CGL_GEOM_SEGMENT_RING_H__
#define __CGL_GEOM_SEGMENT_RING_H__

#include "gl/glew.h"
#include "gl/util/Geometry.h"

namespace gl
{
	class SegmentRing
	{
	public:
		SegmentRing(int segments, float inner_radius, float outer_radius);
		Geometry triangles();

	private:
		int segments_;
		float inner_radius_;
		float outer_radius_;
	};
}

#endif // __CGL_GEOM_SEGMENT_RING_H__