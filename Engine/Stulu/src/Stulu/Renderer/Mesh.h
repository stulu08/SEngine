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
	};
	struct SkinnedVertex {
		glm::vec3 pos = glm::vec3(0.0f);
		glm::vec3 normal = glm::vec3(0.0f);
		glm::vec2 texCoords = glm::vec3(0.0f);
		glm::vec4 color = glm::vec4(1.0f);
		glm::ivec4 boneIds = glm::ivec4(0);
		glm::vec4 weights = glm::vec4(0.0f);
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

		// vertices count if how my vertices are inside the array, the size of this array should be verticesCount * layoutStride
		void SetData(const ByteType* vertices, size_t verticesCount, const uint32_t* indices, uint32_t indicesCount, const BufferLayout& layout);
		void SetData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		void SetData(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t>& indices);

		void AddSubmesh(uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset, const std::string& name = "");
		void AddSubmesh(const MeshSubmesh& sm);

		void RenderCommandDraw(int32_t subMesh = 0, uint32_t instanceCount = 0) const;

		void RecalculateBounds();
		void CalculateNormals();

		const Ref<VertexArray>& GetVertexArray() const { return m_vertexArray; }
		const std::vector<MeshSubmesh>& GetSubmeshes() const { return m_submeshes; }

		const ByteType* GetVertices() const { return m_vertices; }
		const size_t GetVerticesCount() const { return m_verticesCount; }
		const std::vector<uint32_t>& GetIndices() const { return m_indices; }

		const std::string& GetName(int32_t index = -1) const;

		const Ref<BoundingBox>& GetBoundingBox() const { return m_bounds; }

		bool IsDefaultLayout() const { return m_defaultVertexLayout; }
		bool IsDefaultSkinnedLayout() const { return m_defaultSkinnedVertexLayout; }

		std::pair<size_t, BufferElement> GetPositionLayoutElement() const;
		size_t GetStride() const;

		static const BufferLayout DefaultVertexLayout();
		static const BufferLayout DefaultSkinnedVertexLayout();
	private:
		Ref<VertexArray> m_vertexArray;
		Ref<BoundingBox> m_bounds;

		std::string m_name;
		ByteType* m_vertices;
		size_t m_verticesCount;

		std::vector<uint32_t> m_indices;

		std::vector<MeshSubmesh> m_submeshes;

		bool m_defaultVertexLayout : 1;
		bool m_defaultSkinnedVertexLayout : 1;

		void ConstructMesh(const BufferLayout& layout);

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

	inline std::pair<size_t, BufferElement> Mesh::GetPositionLayoutElement() const {
		if (!m_vertexArray) return { 0, BufferElement(ShaderDataType::none, "") };
		if (m_vertexArray->getVertexBuffers().size() < 1) return { 0, BufferElement(ShaderDataType::none, "") };


		if (m_defaultVertexLayout || m_defaultSkinnedVertexLayout) {
			return { 0, *m_vertexArray->getVertexBuffers().front()->getLayout().begin() };
		}

		size_t offset = 0;
		for (const auto& buffer : m_vertexArray->getVertexBuffers()) {
			for (const auto& element : buffer->getLayout()) {
				if (element.name.find("pos") != element.name.npos) {
					return { offset + element.offset, element };
				}
			}
			offset += buffer->getStride();
		}

		return { 0, BufferElement(ShaderDataType::none, "") };
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
			{ Stulu::ShaderDataType::Float3, "a_pos" },
			{ Stulu::ShaderDataType::Float3, "a_normal", true },
			{ Stulu::ShaderDataType::Float2, "a_texCoord" },
			{ Stulu::ShaderDataType::Float4, "a_color" },
		};
	}
	inline const BufferLayout Mesh::DefaultSkinnedVertexLayout() {
		return {
			{ Stulu::ShaderDataType::Float3, "a_pos" },
			{ Stulu::ShaderDataType::Float3, "a_normal", true },
			{ Stulu::ShaderDataType::Float2, "a_texCoord" },
			{ Stulu::ShaderDataType::Float4, "a_color" },
			{ Stulu::ShaderDataType::Int4, "a_boneIds" },
			{ Stulu::ShaderDataType::Float4, "a_weights" },
		};
	}
}

