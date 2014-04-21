#ifndef CGL_VIEWPORT_H_
#define CGL_VIEWPORT_H_

#include "gl/glew.h"
#include "gl/geom/Rectangle.h"

namespace gl
{
    class Viewport : public Rectangle<GLint>
    {
    public:

        /** Applies the viewport using glViewport(). */
		void apply() const {
			glViewport(x, y, width, height);
		}

		Mat4 orthoProjection() const {
			return ortho2D(
				static_cast<float>(left()), 
				static_cast<float>(right()),
				static_cast<float>(bottom()),
				static_cast<float>(top()));
		}
    };
    
} // namespace cgl

#endif // CGL_VIEWPORT_H_
