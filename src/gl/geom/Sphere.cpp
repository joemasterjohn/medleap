#include "Sphere.h"
#include "Box.h"

using namespace gl;

Sphere::Sphere(GLfloat radius, unsigned segments)
{
	Box box(1.0f, segments);
	for (const Vec3& v : box.getVertices())
		vertices.push_back(v.normal() * radius);
	for (const GLuint& i : box.getIndices())
		indices.push_back(i);
}