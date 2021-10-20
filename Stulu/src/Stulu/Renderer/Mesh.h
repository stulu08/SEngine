#pragma once
#include "Stulu/Renderer/Buffer.h"
#include "Stulu/Renderer/VertexArray.h"
#include "glm/glm.hpp"

namespace Stulu {
	class SubMesh {
	public:
		SubMesh() {}
		SubMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

		const std::vector<Vertex>& getVertices() { return m_vertices; }
		const std::vector<uint32_t>& getIndices() { return m_indices; }
		const Ref<Stulu::VertexArray>& getVertexArray() { return m_vertexArray; }
		const size_t& getVerticesCount() { return m_verticesCount; }
		const size_t& getIndicesCount() { return m_indicesCount; }
	private:
		Ref<Stulu::VertexArray> m_vertexArray;
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
		size_t m_verticesCount = 0;
		size_t m_indicesCount = 0;
	};
	class Mesh {
	public:
		Mesh(){}
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

		const void recalculate();

		void setVertices(std::vector<Vertex>& vertices) { m_vertices = vertices; }
		const std::vector<Vertex>& getVertices() { return m_vertices; }

		void setIndices(std::vector<uint32_t>& indices) { m_indices = indices; }
		const std::vector<uint32_t>& getIndices() { return m_indices; }

		SubMesh& getSubMesh(const size_t index) {
			ST_PROFILING_FUNCTION();
			if (index < m_subMeshCount)
				return m_subMeshes[index];

			CORE_ASSERT(false, "Invalid index");
			return m_subMeshes[-1];
		}
		void addSubMesh(const SubMesh mesh) {
			ST_PROFILING_FUNCTION();
			m_subMeshes.push_back(mesh);
			m_subMeshCount++;
		}

		const size_t& getSubMeshCount() { return m_subMeshCount; }
		const size_t& getVerticesCount() { return m_verticesCount; }
		const size_t& getIndicesCount() { return m_indicesCount; }

		static BufferLayout getDefaultLayout() { return m_defaultLayout; }
		const Ref<Stulu::VertexArray>& getVertexArray() { return m_vertexArray; }
	private:
		Ref<Stulu::VertexArray> m_vertexArray = nullptr;
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

		std::vector<SubMesh> m_subMeshes;

		size_t m_subMeshCount = 0;
		size_t m_verticesCount = 0;
		size_t m_indicesCount = 0;

		static BufferLayout m_defaultLayout;
	};
}

