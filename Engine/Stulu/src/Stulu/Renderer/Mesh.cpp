#include "st_pch.h"
#include "Mesh.h"
#include "RenderCommand.h"

namespace Stulu {
	Mesh::Mesh(const std::string& name)
		: m_defaultVertexLayout(false), m_defaultSkinnedVertexLayout(false), m_vertices(nullptr), m_verticesCount(0), m_name(name) {}

	Mesh::~Mesh() {
		if (m_vertices) {
			delete[] m_vertices;
			m_verticesCount = 0;
		}
	}

	void Mesh::SetData(const ByteType* vertices, size_t verticesCount, const uint32_t* indices, uint32_t indicesCount, const BufferLayout& layout)  {
		if (m_vertices) {
			delete[] m_vertices;
			m_verticesCount = 0;
		}

		m_defaultVertexLayout = false;
		m_defaultSkinnedVertexLayout = false;

		const size_t verticesSize = verticesCount * layout.getStride();

		m_vertices = new ByteType[verticesSize];
		m_verticesCount = verticesCount;
		memcpy_s(m_vertices, verticesSize, vertices, verticesSize);

		m_indices = std::vector<uint32_t>(indices, indices + indicesCount);
		ConstructMesh(layout);
	}

	void Mesh::SetData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
		const ByteType* data = (ByteType*)vertices.data();
		SetData(data, vertices.size(), indices.data(), (uint32_t)indices.size(), DefaultVertexLayout());
		m_defaultVertexLayout = true;
	}
	void Mesh::SetData(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t>& indices) {
		const ByteType* data = (ByteType*)vertices.data();
		SetData(data, vertices.size(), indices.data(),(uint32_t)indices.size(), DefaultSkinnedVertexLayout());
		m_defaultSkinnedVertexLayout = true;
	}

	void Mesh::ConstructMesh(const BufferLayout& layout) {
		const size_t verticesSize = m_verticesCount * layout.getStride();

		auto vb = VertexBuffer::create((uint32_t)verticesSize, m_vertices);
		vb->setLayout(layout);

		auto ib = IndexBuffer::create((uint32_t)m_indices.size(), m_indices.data());

		m_vertexArray = VertexArray::create();
		m_vertexArray->setIndexBuffer(ib);
		m_vertexArray->addVertexBuffer(vb);

		RecalculateBounds();
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
	void Mesh::RecalculateBounds() {
		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

		const auto [positionOffset, positionElement] = GetPositionLayoutElement();
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
		m_bounds = VFC::createBoundingBox({ min,max });
	}

	void Mesh::CalculateNormals() {
		if (!m_vertices || m_verticesCount == 0)
			return;

		if (m_defaultVertexLayout) {
			CalculateNormalsInternal((Vertex*)m_vertices);
			ConstructMesh(DefaultVertexLayout());
		}
		else if (m_defaultSkinnedVertexLayout) {
			CalculateNormalsInternal((SkinnedVertex*)m_vertices);
			ConstructMesh(DefaultVertexLayout());
		}
	}
}