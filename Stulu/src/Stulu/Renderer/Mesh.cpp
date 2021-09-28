#include "st_pch.h"
#include "Mesh.h"

namespace Stulu {
	BufferLayout Mesh::m_defaultLayout = {
		{ Stulu::ShaderDataType::Float3, "a_pos" },
		{ Stulu::ShaderDataType::Float3, "a_normal" },
		{ Stulu::ShaderDataType::Float2, "a_texCoord" },
	};


	Mesh::Mesh(std::vector<float> vertexArray, std::vector<uint32_t> indices, BufferLayout layout){
		ST_PROFILING_FUNCTION();
		Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
		Stulu::Ref<Stulu::IndexBuffer> indexBuffer;


		m_vertexArray = Stulu::VertexArray::create();
		vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(vertexArray.size() * sizeof(float)), vertexArray.data());
		vertexBuffer->setLayout(layout);
		m_vertexArray->addVertexBuffer(vertexBuffer);
		indexBuffer = Stulu::IndexBuffer::create((uint32_t)(indices.size() * sizeof(uint32_t)), indices.data());
		m_vertexArray->setIndexBuffer(indexBuffer);

	}
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
		ST_PROFILING_FUNCTION();
		m_vertices = vertices;
		m_indices = indices;
		recalculate();
	}
	void Mesh::recalculate() {
		ST_PROFILING_FUNCTION();
		Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
		Stulu::Ref<Stulu::IndexBuffer> indexBuffer;

		m_vertexArray = Stulu::VertexArray::create();
		vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(m_vertices.size() * sizeof(Vertex)), &m_vertices[0]);
		vertexBuffer->setLayout(m_defaultLayout);
		m_vertexArray->addVertexBuffer(vertexBuffer);
		indexBuffer = Stulu::IndexBuffer::create((uint32_t)m_indices.size(), m_indices.data());
		m_vertexArray->setIndexBuffer(indexBuffer);
	}
}