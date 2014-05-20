#ifndef __GL_VIEWPORT_H__
#define __GL_VIEWPORT_H__

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

		Vec2 normalize(Vec2 coords) const {
			return{ (coords.x - x) / width, (coords.y - y) / height };
		}

		Mat4 orthoProjection() const {
			return ortho2D(
				0.0f,
				width,
				0.0f,
				height);
		}
    };
    
}

#endif // __GL_VIEWPORT_H__
