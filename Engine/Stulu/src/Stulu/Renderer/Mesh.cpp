#include "st_pch.h"
#include "Mesh.h"
#include "RenderCommand.h"

namespace Stulu {
	Mesh::Mesh(const std::string& name)
		: m_vertices(nullptr), m_verticesCount(0), m_name(name) {
		m_vertexArray = VertexArray::create();
	}

	Mesh::~Mesh() {
		if (m_vertices) {
			delete[] m_vertices;
			m_verticesCount = 0;
		}
	}

	void Mesh::ConstructMesh(const ByteType* vertices, size_t verticesCount, const BufferLayout& layout, const uint32_t* indices, uint32_t indicesCount, bool calcNormals) {
		SetVertices(vertices, verticesCount, layout);
		SetIndices(indices, indicesCount);

		if(calcNormals)
			CalculateNormals(false);

		UploadIndexBuffer();
		UploadVertexBuffer(layout);

		CalculateBounds();
	}

	void Mesh::ConstructMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, bool calcNormals) {
		SetVertices(vertices);
		SetIndices(indices);

		if (calcNormals)
			CalculateNormals(false);

		UploadIndexBuffer();
		UploadVertexBuffer(DefaultVertexLayout());

		CalculateBounds();
	}


	void Mesh::SetVertices(const ByteType* vertices, size_t verticesCount, const BufferLayout& layout) {
		if (m_vertices) {
			delete[] m_vertices;
			m_verticesCount = 0;
		}

		const size_t verticesSize = verticesCount * layout.getStride();

		m_vertices = new ByteType[verticesSize];
		m_verticesCount = verticesCount;
		memcpy_s(m_vertices, verticesSize, vertices, verticesSize);
	}

	void Mesh::UploadVertexBuffer(const BufferLayout& layout) {
		m_vertexArray->clearVertexBuffers();
		const size_t verticesSize = m_verticesCount * layout.getStride();
		auto vb = VertexBuffer::create((uint32_t)verticesSize, m_vertices);
		vb->setLayout(layout);
		m_vertexArray->addVertexBuffer(vb);
	}

	void Mesh::UploadIndexBuffer() {
		auto ib = IndexBuffer::create((uint32_t)m_indices.size(), m_indices.data());
		m_vertexArray->setIndexBuffer(ib);
	}

	void Mesh::AddSubmesh(uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset, const std::string& name) {
		MeshSubmesh mesh;
		mesh.indexOffset = indexOffset;
		mesh.indexCount = indexCount;
		mesh.vertexOffset = vertexOffset;
		mesh.name = name;

		m_submeshes.push_back(mesh);
	}
	void Mesh::AddSubmesh(const MeshSubmesh& sm) {
		m_submeshes.push_back(sm);
	}
	void Mesh::RenderCommandDraw(int32_t subMesh, uint32_t instanceCount) const {
		if (subMesh < 0 || subMesh >= m_submeshes.size()) {
			RenderCommand::drawIndexed(m_vertexArray, 0, instanceCount);
			return;
		}

		const MeshSubmesh& mesh = m_submeshes[subMesh];
		RenderCommand::drawIndexedSubMesh(m_vertexArray, mesh.indexCount, mesh.indexOffset, mesh.vertexOffset, instanceCount);
	}

	const std::string& Mesh::GetName(int32_t index) const {
		if (index < 0 || index >= m_submeshes.size()) {
			return m_name;
		}

		return m_submeshes[index].name;
	}
	const void Mesh::SetName(const std::string& name, int32_t index) {
		if (index < 0 || index >= m_submeshes.size()) {
			m_name = name;
			return;
		}

		m_submeshes[index].name = name;
	}
	void Mesh::CalculateBounds() {
		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

		const BufferElement positionElement = GetPositionLayoutElement();
		const size_t positionOffset = positionElement.offset;
		const size_t stride = GetStride();

		for (size_t i = 0; i < m_verticesCount; i++) {
			ByteType* vertex = m_vertices + (i * stride);

			glm::vec3 pos = glm::vec3(0.0f);

			if (positionElement.type == ShaderDataType::Float3) {
				pos = *((const glm::vec3*)vertex + positionOffset);
			}
			else if (positionElement.type == ShaderDataType::Float4) {
				glm::vec4 vec4Pos = *((const glm::vec4*)vertex + positionOffset);
				pos = { vec4Pos.x, vec4Pos.y, vec4Pos.z };
			}
			else {
				continue;
			}

			//min
			min = glm::min(min, pos);
			max = glm::max(max, pos);
			
		}
		m_bounds = AABB{ min, max };
	}
}