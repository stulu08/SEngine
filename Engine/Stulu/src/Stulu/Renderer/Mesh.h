#pragma once
#include "Stulu/Renderer/Buffer.h"
#include "Stulu/Renderer/VertexArray.h"
#include "Stulu/Types/UUID.h"
#include "Stulu/Scene/VFC.h"

#include "glm/glm.hpp"

namespace Stulu {
	struct Vertex {
		glm::vec3 pos = glm::vec3(0.0f);
		glm::vec3 normal = glm::vec3(0.0f);
		glm::vec2 texCoords = glm::vec3(0.0f);
		glm::vec4 color = glm::vec4(1.0f);
		glm::vec4 weights = glm::vec4(0.0f);
		glm::ivec4 boneIds = glm::ivec4(0);
	};

	struct MeshSubmesh {
		// Where this submesh's indices start in the global index buffer
		uint32_t indexOffset = 0;
		// How many indices it has (i.e., how many triangles * 3)
		uint32_t indexCount = 0;
		// Where its vertices start in the vertex buffer (for skinning or other per-submesh use)
		uint32_t vertexOffset = 0;
		std::string name;
	};
	// Make sure that the position field inside the vertex layout is named a_pos, only then can the physics api use this mesh
	class STULU_API Mesh {
	public:
		using ByteType = std::byte;

		Mesh(const std::string& name = "");
		~Mesh();

		// Does everything, SetVertices(), SetIndices(), CalculateBounds(), CalculateNormals(), UploadVertexBuffer(), UploadIndexBuffer()
		void ConstructMesh(const ByteType* vertices, size_t verticesCount, const BufferLayout& layout, const uint32_t* indices, uint32_t indicesCount, bool calcNormals = false);
		void ConstructMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, bool calcNormals = false);

		// Size of vertices = verticesCount * layoutStride
		void SetVertices(const ByteType* vertices, size_t verticesCount, const BufferLayout& layout);
		void SetVertices(const std::vector<Vertex>& vertices);
		void SetVertices(const Vertex* vertices, size_t count);

		void SetIndices(const uint32_t* indices, uint32_t indicesCount);
		void SetIndices(const std::vector<uint32_t>& indices);

		void UploadVertexBuffer(const BufferLayout& layout);
		void UploadIndexBuffer();

		void AddSubmesh(uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset, const std::string& name = "");
		void AddSubmesh(const MeshSubmesh& sm);

		void RenderCommandDraw(int32_t subMesh = 0, uint32_t instanceCount = 0) const;

		void CalculateBounds();
		template<class T = Vertex>
		void CalculateNormals(bool upload = true);

		const Ref<VertexArray>& GetVertexArray() const { return m_vertexArray; }
		std::vector<MeshSubmesh>& GetSubmeshes() { return m_submeshes; }
		const std::vector<MeshSubmesh>& GetSubmeshes() const { return m_submeshes; }

		const ByteType* GetVertices() const { return m_vertices; }
		const size_t GetVerticesCount() const { return m_verticesCount; }
		const std::vector<uint32_t>& GetIndices() const { return m_indices; }

		const std::string& GetName(int32_t index = -1) const;
		const void SetName(const std::string& name, int32_t index = -1);

		const BoundingBox& GetBoundingBox() const { return m_bounds; }

		BufferElement GetPositionLayoutElement() const;
		size_t GetStride() const;

		static const BufferLayout DefaultVertexLayout();
	private:
		Ref<VertexArray> m_vertexArray;
		BoundingBox m_bounds;

		std::string m_name;
		ByteType* m_vertices;
		size_t m_verticesCount;

		std::vector<uint32_t> m_indices;

		std::vector<MeshSubmesh> m_submeshes;


		template<typename TVertex>
		inline void CalculateNormalsInternal(TVertex* vertices) {
			for (size_t i = 0; i < m_verticesCount; ++i)
				vertices[i].normal = glm::vec3(0.0f);

			for (size_t i = 0; i < m_indices.size(); i += 3) {
				glm::vec3 p0 = vertices[m_indices[i]].pos;
				glm::vec3 p1 = vertices[m_indices[i + 1]].pos;
				glm::vec3 p2 = vertices[m_indices[i + 2]].pos;

				glm::vec3 normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));

				vertices[m_indices[i]].normal += normal;
				vertices[m_indices[i + 1]].normal += normal;
				vertices[m_indices[i + 2]].normal += normal;
			}

			for (size_t i = 0; i < m_verticesCount; ++i)
				vertices[i].normal = glm::normalize(vertices[i].normal);
		}

		friend class SharedMeshAssetData;
	};

	template<class T>
	inline void Mesh::CalculateNormals(bool upload) {
		if (!m_vertices || m_verticesCount == 0)
			return;

		CalculateNormalsInternal((T*)m_vertices);
		if (upload)
			UploadVertexBuffer(DefaultVertexLayout());
	}

	inline BufferElement Mesh::GetPositionLayoutElement() const {
		if (!m_vertexArray) return EmptyBufferElement;
		if (m_vertexArray->getVertexBuffers().size() < 1) return EmptyBufferElement;


		for (const auto& buffer : m_vertexArray->getVertexBuffers()) {
			for (const auto& element : buffer->getLayout()) {
				if (element.idType == BufferElementIDType::Position)
					return element;
			}
		}
		return EmptyBufferElement;
	}

	inline size_t Mesh::GetStride() const {
		if (m_vertexArray) {
			size_t stride = 0;
			for (const auto& buffer : m_vertexArray->getVertexBuffers()) {
				stride += buffer->getStride();
			}
			return stride;
		}
		return 0;
	}

	inline const BufferLayout Mesh::DefaultVertexLayout() {
		return {
			PositionBufferElement,
			NormalBufferElement,
			TextureCoordsBufferElement,
			ColorBufferElement,
			BoneWeightsBufferElement,
			BoneIndicesBufferElement
		};
	}

	inline void Mesh::SetVertices(const std::vector<Vertex>& vertices) {
		const ByteType* data = (ByteType*)vertices.data();
		SetVertices(data, vertices.size(), DefaultVertexLayout());
	}
	inline void Mesh::SetVertices(const Vertex* vertices, size_t count) {
		SetVertices((ByteType*)vertices, count, DefaultVertexLayout());
	}


	inline void Mesh::SetIndices(const uint32_t* indices, uint32_t indicesCount) {
		m_indices = std::vector<uint32_t>(indices, indices + indicesCount);
	}
	inline void Mesh::SetIndices(const std::vector<uint32_t>& indices) {
		SetIndices(indices.data(), (uint32_t)indices.size());
	}
}

