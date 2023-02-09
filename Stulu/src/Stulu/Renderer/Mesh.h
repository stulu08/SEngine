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
		Mesh();
		~Mesh();
		Mesh(const Mesh&) = default;
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		Mesh(Vertex* vertices, uint32_t verticesCount, uint32_t* indices, uint32_t indicesCount);
		Mesh(void* vertices, uint32_t verticesSize, uint32_t* indices, uint32_t indicesCount, const BufferLayout& layout = s_defaultLayout);

		const void recalculate();
		//use only with the default vertex
		const void calculateNormals();

		void setVertices(const std::vector<Vertex>& vertices, const BufferLayout& layout = s_defaultLayout);
		void setVertices(void* vertices, uint32_t verticesSize, const BufferLayout& layout = s_defaultLayout);
		void setIndices(const std::vector<uint32_t>& indices);


		SubMesh& getSubMesh(const size_t index) {
			if (index < m_subMeshCount)
				return m_subMeshes[index];

			CORE_ASSERT(false, "Invalid index");
			return m_subMeshes[-1];
		}
		void addSubMesh(const SubMesh mesh) {
			m_subMeshes.push_back(mesh);
			m_subMeshCount++;
		}

		uint32_t getSubMeshCount() const { return m_subMeshCount; }
		const Ref<BoundingBox>& getBoundingBox() const { return m_boundingBox; }


		const Ref<Stulu::VertexArray>& getVertexArray() const { return m_vertexArray; }
		const Ref<Stulu::VertexBuffer>& getVertexBuffer() const { return m_vertexArray->getVertexBuffers()[0]; }
		uint32_t getVerticesCount() const { return m_verticesCount; }
		const std::vector<Vertex>& getVertices() const { return m_vertices; }
		const void* getVertexData() const { return m_vertexData; }
		std::vector<Vertex>& getVertices() { return m_vertices; }
		void* getVertexData() { return m_vertexData; }

		const Ref<Stulu::IndexBuffer>& getIndexBuffer() const { return m_vertexArray->getIndexBuffer(); }
		uint32_t getIndicesCount() const { return m_indicesCount; }
		const std::vector<uint32_t>& getIndices() const { return m_indices; }
		const void* getIndicesData() const { return m_indicesData; }
		std::vector<uint32_t>& getIndices() { return m_indices; }
		void* getIndicesData() { return m_indicesData; }


		Vertex getFurthestVertexFromPos(const glm::vec3& pos, uint64_t scanCount = 0) const;
		glm::vec3 getFurthesteachAxisFromPos(const glm::vec3& pos, uint64_t scanCount = 0) const;



		static BufferLayout getDefaultLayout() { return s_defaultLayout; }
		//for generating a convex mesh
		static Mesh copyAndLimit(const Ref<Mesh>& mesh, uint64_t vertLimit = 0);

		static Mesh combine(const Mesh& mesh);
		static Mesh combine(const Ref<Mesh>& mesh);
	private:
		Ref<Stulu::BoundingBox> m_boundingBox = nullptr;
		Ref<Stulu::VertexArray> m_vertexArray = nullptr;

		std::vector<Vertex> m_vertices;
		uint32_t m_verticesCount = 0;
		void* m_vertexData = nullptr;
		
		std::vector<uint32_t> m_indices;
		uint32_t m_indicesCount = 0;
		uint32_t* m_indicesData = nullptr;

		std::vector<SubMesh> m_subMeshes;
		uint32_t m_subMeshCount = 0;

		static BufferLayout s_defaultLayout;

		friend class PhysX;
	};
}

