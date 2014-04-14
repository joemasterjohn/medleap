#ifndef CGL_SPHERE_H_
#define CGL_SPHERE_H_

#include "gl/glew.h"
#include "gl/Buffer.h"
#include "gl/math/Vector3.h"
#include <vector>

namespace gl
{
    class Sphere
    {
    public:
		Sphere(GLfloat edgeLength);
		Sphere(GLfloat width, GLfloat height, GLfloat depth);
		void fill(gl::Buffer& vertexBuffer) const;
		void fill(gl::Buffer& vertexBuffer, gl::Buffer& indexBuffer) const;
		const std::vector<Vec3>& getVertices() const;
		const std::vector<GLushort>& getIndices() const;

	private:
		std::vector<Vec3> vertices;
		std::vector<GLushort> indices;
    };
    
} // namespace cgl

#endif // CGL_SPHERE_H_