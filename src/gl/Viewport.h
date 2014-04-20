#ifndef CGL_VIEWPORT_H_
#define CGL_VIEWPORT_H_

#include "gl/glew.h"
#include "gl/geom/Rectangle.h"
#include "gl/math/Transform.h"

namespace gl
{
    class Viewport : public Rectangle<int>
    {
    public:

        /** Applies the viewport using glViewport(). */
		void apply() const {
			glViewport(x, y, width, height);
		}

		Mat4 orthoProjection() const {
			return ortho2D(left(), right(), bottom(), top());
		}
    };
    
} // namespace cgl

#endif // CGL_VIEWPORT_H_
