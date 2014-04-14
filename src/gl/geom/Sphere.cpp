#include "Sphere.h"
#include <algorithm>
using namespace gl;

Sphere::Sphere(GLfloat edgeLength) : Sphere(edgeLength, edgeLength, edgeLength)
{
}

Sphere::Sphere(GLfloat width, GLfloat height, GLfloat depth)
{
	float x = width / 2.0f;
	float y = height / 2.0f;
	float z = depth / 2.0f;

	vertices.push_back(Vec3(-x, -y, +z));
	vertices.push_back(Vec3(+x, -y, +z));
	vertices.push_back(Vec3(+x, +y, +z));
	vertices.push_back(Vec3(-x, +y, +z));
	vertices.push_back(Vec3(-x, -y, -z));
	vertices.push_back(Vec3(+x, -y, -z));
	vertices.push_back(Vec3(+x, +y, -z));
	vertices.push_back(Vec3(-x, +y, -z));

	auto face = [&](GLushort i, GLushort j, GLushort k, GLushort l) {
		indices.push_back(i);
		indices.push_back(j);
		indices.push_back(k);
		indices.push_back(i);
		indices.push_back(k);
		indices.push_back(l);
	};

	face(0, 1, 2, 3);
	face(1, 5, 6, 2);
	face(5, 4, 7, 6);
	face(4, 0, 3, 7);
	face(3, 2, 6, 7);
	face(0, 4, 5, 1);
}

void Sphere::fill(Buffer& vertexBuffer) const
{
	std::vector<Vec3> buf;
	for (GLushort index : indices)
		buf.push_back(vertices[index]);
	
	vertexBuffer.bind();
	vertexBuffer.setData(&buf[0], buf.size() * sizeof(Vec3));
}

void Sphere::fill(Buffer& vertexBuffer, Buffer& indexBuffer) const
{
	vertexBuffer.bind();
	vertexBuffer.setData(&vertices[0], vertices.size() * sizeof(Vec3));
	indexBuffer.bind();
	indexBuffer.setData(&indices[0], indices.size() * sizeof(GLushort));
}

const std::vector<Vec3>& Sphere::getVertices() const
{
	return vertices;
}

const std::vector<GLushort>& Sphere::getIndices() const
{
	return indices;
}