#include "st_pch.h"
#include "Mesh.h"

namespace Stulu {
	BufferLayout Mesh::s_defaultLayout = {
		{ Stulu::ShaderDataType::Float3, "a_pos" },
		{ Stulu::ShaderDataType::Float3, "a_normal" },
		{ Stulu::ShaderDataType::Float2, "a_texCoord" },
	};

	SubMesh::SubMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
		ST_PROFILING_FUNCTION();
		m_vertices = vertices;
		m_indices = indices;
		m_verticesCount = vertices.size();
		m_indicesCount = indices.size();
		Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
		Stulu::Ref<Stulu::IndexBuffer> indexBuffer;

		m_vertexArray = Stulu::VertexArray::create();
		vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(m_verticesCount * sizeof(Vertex)), &vertices[0]);
		vertexBuffer->setLayout(Mesh::getDefaultLayout());
		m_vertexArray->addVertexBuffer(vertexBuffer);
		indexBuffer = Stulu::IndexBuffer::create((uint32_t)m_indicesCount, indices.data());
		m_vertexArray->setIndexBuffer(indexBuffer);
	}

	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
		ST_PROFILING_FUNCTION();
		m_vertices = vertices;
		m_indices = indices;
		recalculate();
	}
	Mesh::Mesh(Vertex* vertices, size_t verticesCount, uint32_t* indices, size_t indicesCount) {
		ST_PROFILING_FUNCTION();
		Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
		Stulu::Ref<Stulu::IndexBuffer> indexBuffer;
		m_vertexArray = Stulu::VertexArray::create();
		vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(verticesCount * sizeof(Vertex)), vertices);
		vertexBuffer->setLayout(s_defaultLayout);
		m_vertexArray->addVertexBuffer(vertexBuffer);
		indexBuffer = Stulu::IndexBuffer::create((uint32_t)indicesCount, indices);
		m_vertexArray->setIndexBuffer(indexBuffer);
		m_vertices = std::vector<Vertex>(vertices, vertices + verticesCount);
		m_indices = std::vector<uint32_t>(indices, indices + indicesCount);
		m_verticesCount = verticesCount;
		m_indicesCount = indicesCount;
	}
	Mesh::Mesh(void* vertices, size_t verticesSize, uint32_t* indices, size_t indicesCount, BufferLayout layout) {
		Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
		Stulu::Ref<Stulu::IndexBuffer> indexBuffer;
		m_vertexArray = Stulu::VertexArray::create();
		vertexBuffer = Stulu::VertexBuffer::create((uint32_t)verticesSize, vertices);
		vertexBuffer->setLayout(layout);
		m_vertexArray->addVertexBuffer(vertexBuffer);
		indexBuffer = Stulu::IndexBuffer::create((uint32_t)indicesCount, indices);
		m_vertexArray->setIndexBuffer(indexBuffer);
	}
	const void Mesh::recalculate() {
		ST_PROFILING_FUNCTION();
		m_verticesCount = m_vertices.size();
		m_indicesCount = m_indices.size();
		Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
		Stulu::Ref<Stulu::IndexBuffer> indexBuffer;

		m_vertexArray = Stulu::VertexArray::create();
		vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(m_verticesCount * sizeof(Vertex)), &m_vertices[0]);
		vertexBuffer->setLayout(s_defaultLayout);
		m_vertexArray->addVertexBuffer(vertexBuffer);
		indexBuffer = Stulu::IndexBuffer::create((uint32_t)m_indicesCount, m_indices.data());
		m_vertexArray->setIndexBuffer(indexBuffer);
	}
	Mesh Mesh::combine(Mesh& mesh) {
		if (mesh.m_subMeshCount == 0)
			return mesh;

		Mesh newMesh;
		uint32_t indicesOffset = 0;

		for (auto& sub : mesh.m_subMeshes) {
			indicesOffset =(uint32_t)newMesh.m_vertices.size();
			for (auto& vertex : sub.m_vertices) {
				newMesh.m_vertices.push_back(vertex);
			}
			for (auto& index : sub.m_indices) {
				newMesh.m_indices.push_back(index + indicesOffset);
			}
		}
		newMesh.recalculate();
		newMesh.m_subMeshes.clear();
		return newMesh;
	}
}