#ifndef CGL_BOX_H_
#define CGL_BOX_H_

#include "gl/glew.h"
#include "gl/Buffer.h"
#include "gl/math/Vector3.h"
#include <vector>

namespace gl
{
    class Box
    {
    public:
		Box(GLfloat edgeLength);
		Box(GLfloat width, GLfloat height, GLfloat depth);
		void fill(gl::Buffer& vertexBuffer) const;
		void fill(gl::Buffer& vertexBuffer, gl::Buffer& indexBuffer) const;
		const std::vector<Vec3>& getVertices() const;
		const std::vector<GLushort>& getIndices() const;

	private:
		std::vector<Vec3> vertices;
		std::vector<GLushort> indices;
    };
    
} // namespace cgl

#endif // CGL_BOX_H_