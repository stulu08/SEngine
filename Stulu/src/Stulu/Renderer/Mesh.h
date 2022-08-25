#pragma once
#include "Stulu/Renderer/Buffer.h"
#include "Stulu/Renderer/VertexArray.h"
#include "Stulu/Core/UUID.h"
#include "glm/glm.hpp"
#include <Stulu/Scene/VFC.h>

namespace Stulu {
	class STULU_API SubMesh {
	public:
		SubMesh() {}
		SubMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		const Ref<Stulu::VertexArray>& getVertexArray() const { return m_vertexArray; }
		const size_t& getVerticesCount() const { return m_verticesCount; }
		const std::vector<Vertex>& getVertices() const { return m_vertices; }
		const size_t& getIndicesCount() const { return m_indicesCount; }
		const std::vector<uint32_t>& getIndices() const { return m_indices; }
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
		Mesh() = default;
		Mesh(const Mesh&) = default;
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		Mesh(Vertex* vertices, size_t verticesCount, uint32_t* indices, size_t indicesCount);
		Mesh(void* vertices, size_t verticesSize, uint32_t* indices, size_t indicesCount, BufferLayout layout = s_defaultLayout);

		const void recalculate();
		const void calculateNormals();

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

		size_t getSubMeshCount() const { return m_subMeshCount; }
		size_t getVerticesCount() const { return m_verticesCount; }
		size_t getIndicesCount() const { return m_indicesCount; }
		const std::vector<Vertex>& getVertices() const { return m_vertices; }
		const std::vector<uint32_t>& getIndices() const { return m_indices; }
		std::vector<Vertex>& getVertices() { return m_vertices; }
		std::vector<uint32_t>& getIndices() { return m_indices; }

		static BufferLayout getDefaultLayout() { return s_defaultLayout; }
		const Ref<Stulu::VertexArray>& getVertexArray() const { return m_vertexArray; }
		const Ref<BoundingBox>& getBoundingBox() const { return m_boundingBox; }

		Vertex getFurthestVertexFromPos(const glm::vec3& pos, uint64_t scanCount = 0) const;
		glm::vec3 getFurthesteachAxisFromPos(const glm::vec3& pos, uint64_t scanCount = 0) const;

		//for generating a convex mesh
		static Mesh copyAndLimit(const Ref<Mesh>& mesh, uint64_t vertLimit = 0);

		static Mesh combine(const Mesh& mesh);
		static Mesh combine(const Ref<Mesh>& mesh);
	private:
		Ref<Stulu::VertexArray> m_vertexArray = nullptr;
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

		std::vector<SubMesh> m_subMeshes;
		size_t m_subMeshCount = 0;
		size_t m_verticesCount = 0;
		size_t m_indicesCount = 0;

		Ref<BoundingBox> m_boundingBox = nullptr;

		static BufferLayout s_defaultLayout;

		friend class PhysX;
	};
}

