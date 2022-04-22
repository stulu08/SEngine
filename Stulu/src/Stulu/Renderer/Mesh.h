#pragma once
#include "Stulu/Renderer/Buffer.h"
#include "Stulu/Renderer/VertexArray.h"
#include "Stulu/Core/UUID.h"
#include "glm/glm.hpp"

namespace Stulu {
	class STULU_API SubMesh {
	public:
		SubMesh() {}
		SubMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		const Ref<Stulu::VertexArray>& getVertexArray() { return m_vertexArray; }
		const size_t& getVerticesCount() { return m_verticesCount; }
		const std::vector<Vertex>& getVertices() { return m_vertices; }
		const size_t& getIndicesCount() { return m_indicesCount; }
		const std::vector<uint32_t>& getIndices() { return m_indices; }
	private:
		Ref<Stulu::VertexArray> m_vertexArray;
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
		size_t m_verticesCount = 0;
		size_t m_indicesCount = 0;

		friend class Mesh;
	};
	class STULU_API Mesh {
	public:
		Mesh() {}
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		Mesh(Vertex* vertices, size_t verticesCount, uint32_t* indices, size_t indicesCount);
		Mesh(void* vertices, size_t verticesSize, uint32_t* indices, size_t indicesCount, BufferLayout layout = s_defaultLayout);

		const void recalculate();

		void setVertices(std::vector<Vertex>& vertices) { m_vertices = vertices; }

		void setIndices(std::vector<uint32_t>& indices) { m_indices = indices; }

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
		const std::vector<Vertex>& getVertices() { return m_vertices; }
		const size_t& getIndicesCount() { return m_indicesCount; }
		const std::vector<uint32_t>& getIndices() { return m_indices; }

		static BufferLayout getDefaultLayout() { return s_defaultLayout; }
		const Ref<Stulu::VertexArray>& getVertexArray() { return m_vertexArray; }

		static Mesh combine(Mesh& mesh);
	private:
		Ref<Stulu::VertexArray> m_vertexArray = nullptr;
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

		std::vector<SubMesh> m_subMeshes;
		size_t m_subMeshCount = 0;
		size_t m_verticesCount = 0;
		size_t m_indicesCount = 0;

		static BufferLayout s_defaultLayout;

		friend class PhysX;
	};
}

