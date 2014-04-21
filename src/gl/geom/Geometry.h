#ifndef __CGL_GEOMETRY_H__
#define __CGL_GEOMETRY_H__

#include "gl/glew.h"
#include "gl/Buffer.h"
#include "gl/math/Vector3.h"
#include <vector>

namespace gl
{
	template <class VertexType, class IndexType>
    class Geometry
    {
    	public:
			virtual ~Geometry() {}

			void fill(gl::Buffer& vertexBuffer) const {
				std::vector<VertexType> buf;
				for (IndexType index : indices)
					buf.push_back(vertices[index]);

				vertexBuffer.bind();
				vertexBuffer.data(&buf[0], buf.size() * sizeof(VertexType));
			}

			void fill(gl::Buffer& vertexBuffer, gl::Buffer& indexBuffer) {
				vertexBuffer.bind();
				vertexBuffer.data(&vertices[0], vertices.size() * sizeof(VertexType));
				indexBuffer.bind();
				indexBuffer.data(&indices[0], indices.size() * sizeof(IndexType));
			}

			const std::vector<VertexType>& getVertices() const {
				return vertices;
			}

			const std::vector<IndexType>& getIndices() const {
				return indices;
			}

		protected:
			std::vector<VertexType> vertices;
			std::vector<IndexType> indices;
    };
    
} // namespace cgl

#endif // __CGL_GEOMETRY_H__