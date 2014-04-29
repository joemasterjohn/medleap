#include "Geometry.h"

using namespace gl;

Geometry::Geometry() : mode(GL_TRIANGLES)
{
}

Geometry::Geometry(Geometry&& geometry)
{
	*this = std::move(geometry);
}

Geometry& Geometry::operator=(Geometry&& geometry)
{
	std::swap(vertices, geometry.vertices);
	std::swap(indices, geometry.indices);
	mode = geometry.mode;
	return *this;
}