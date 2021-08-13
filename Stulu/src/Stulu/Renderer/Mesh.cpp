#include "st_pch.h"
#include "Mesh.h"

namespace Stulu {
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
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, BufferLayout layout) {
		ST_PROFILING_FUNCTION();
		Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
		Stulu::Ref<Stulu::IndexBuffer> indexBuffer;


		m_vertexArray = Stulu::VertexArray::create();
		vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(vertices.size() * sizeof(Vertex)), &vertices[0]);
		vertexBuffer->setLayout(layout);
		m_vertexArray->addVertexBuffer(vertexBuffer);
		indexBuffer = Stulu::IndexBuffer::create((uint32_t)indices.size(), indices.data());
		m_vertexArray->setIndexBuffer(indexBuffer);

	}
}