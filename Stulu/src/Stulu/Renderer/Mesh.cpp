#include "st_pch.h"
#include "Mesh.h"

namespace Stulu {
	BufferLayout Mesh::s_defaultLayout = {
		{ Stulu::ShaderDataType::Float3, "a_pos" },
		{ Stulu::ShaderDataType::Float3, "a_normal" },
		{ Stulu::ShaderDataType::Float2, "a_texCoord" },
		{ Stulu::ShaderDataType::Float4, "a_color" },
	};

	SubMesh::SubMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
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
		indexBuffer = Stulu::IndexBuffer::create((uint32_t)m_indicesCount, m_indices.data());
		m_vertexArray->setIndexBuffer(indexBuffer);
	}
	Mesh::Mesh() {
		ST_PROFILING_FUNCTION();
		m_vertexArray = Stulu::VertexArray::create();
		recalculate();
	}
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
		ST_PROFILING_FUNCTION();
		m_vertexArray = Stulu::VertexArray::create();
		setVertices(vertices);
		setIndices(indices);
		recalculate();
	}
	Mesh::Mesh(Vertex* vertices, uint32_t verticesCount, uint32_t* indices, uint32_t indicesCount) {
		ST_PROFILING_FUNCTION();
		m_vertexArray = Stulu::VertexArray::create();
		std::vector<Vertex> vertVec(vertices, vertices + verticesCount);
		std::vector<uint32_t> indicVec(indices, indices + indicesCount);
		setVertices(vertVec);
		setIndices(indicVec);
		recalculate();
	}
	Mesh::Mesh(void* vertices, uint32_t verticesSize, uint32_t* indices, uint32_t indicesCount, const BufferLayout& layout) {
		ST_PROFILING_FUNCTION();
		m_vertexArray = Stulu::VertexArray::create();
		std::vector<uint32_t> indicVec(indices, indices + indicesCount);
		setVertices(vertices, verticesSize, layout);
		setIndices(indicVec);
		recalculate();
	}

	void Mesh::setVertices(const std::vector<Vertex>& vertices, const BufferLayout& layout) {
		ST_PROFILING_FUNCTION();
		if (!m_vertexArray->getVertexBuffers().empty())
			m_vertexArray->clearVertexBuffers();

		m_vertices.clear();
		m_vertices = vertices;
		m_verticesCount = (uint32_t)m_vertices.size();
		m_vertexData = m_vertices.data();

		Stulu::Ref<Stulu::VertexBuffer> vertexBuffer = VertexBuffer::create(layout.getStride() * m_verticesCount, m_vertexData);
		vertexBuffer->setLayout(layout);
		m_vertexArray->addVertexBuffer(vertexBuffer);
	}
	void Mesh::setVertices(void* vertices, uint32_t verticesSize, const BufferLayout& layout) {
		ST_PROFILING_FUNCTION();
		if (!m_vertexArray->getVertexBuffers().empty())
			m_vertexArray->clearVertexBuffers();

		m_vertices.clear();
		m_verticesCount = verticesSize / getVertexBuffer()->getStride();
		m_vertexData = vertices;

		Stulu::Ref<Stulu::VertexBuffer> vertexBuffer = VertexBuffer::create(verticesSize, vertices);
		vertexBuffer->setLayout(layout);
		m_vertexArray->addVertexBuffer(vertexBuffer);
	}
	void Mesh::setIndices(const std::vector<uint32_t>& indices) {
		ST_PROFILING_FUNCTION();
		m_indices = indices;
		m_indicesCount = (uint32_t)m_indices.size();
		m_indicesData = m_indices.data();

		Stulu::Ref<Stulu::IndexBuffer> indexBuffer = IndexBuffer::create(m_indicesCount, m_indicesData);
		m_vertexArray->setIndexBuffer(indexBuffer);
	}


	const void Mesh::recalculate() {
		ST_PROFILING_FUNCTION();
		m_boundingBox = VFC::createBoundingBox(this);
	}
	const void Mesh::calculateNormals() {
		ST_PROFILING_FUNCTION();
		size_t indexCount = m_indices.size();

		std::vector<Vertex> vertices = m_vertices;

		vertices.reserve(vertices.size());

		for (int i = 0; i < indexCount; i += 3)
		{
			// get the three vertices that make the faces
			glm::vec3 p0 = vertices[m_indices[i + 0]].pos;
			glm::vec3 p1 = vertices[m_indices[i + 1]].pos;
			glm::vec3 p2 = vertices[m_indices[i + 2]].pos;

			glm::vec3 e1 = p1 - p0;
			glm::vec3 e2 = p2 - p0;
			glm::vec3 normal = glm::cross(e1, e2);
			normal = glm::normalize(normal);

			// Store the face's normal for each of the vertices that make up the face.
			vertices[m_indices[i + 0]].normal += normal;
			vertices[m_indices[i + 1]].normal += normal;
			vertices[m_indices[i + 2]].normal += normal;
		}


		// Now loop through each vertex vector, and avarage out all the normals stored.
		for (int i = 0; i < vertices.size(); ++i)
		{
			vertices[i].normal = glm::normalize(vertices[i].normal);
		}
		vertices.reserve(vertices.size());

		setVertices(vertices);
	}

	Vertex Mesh::getFurthestVertexFromPos(const glm::vec3& pos, uint64_t vertLimit) const {
		ST_PROFILING_FUNCTION();
		if (vertLimit == 0 || vertLimit > getVerticesCount())
			vertLimit = getVerticesCount();

		uint32_t skipCount = (uint32_t)glm::ceil(getVerticesCount() / vertLimit);

		Vertex furthest = Vertex{ glm::vec3(.0f),glm::vec3(.0f),glm::vec2(.0f) };

		float dist = 0;
		if (getVertices().size() > 0)
			dist = glm::distance(getVertices()[0].pos, pos);

		for (uint32_t i = 0, j = 0; i < getVerticesCount(); i += skipCount) {
			if (j >= vertLimit)
				break;
			if (dist < glm::distance(getVertices()[i].pos, pos)) {
				furthest = getVertices()[i];
				dist = glm::distance(getVertices()[i].pos, pos);
			}
			j++;
		}
		return furthest;
	}
	glm::vec3 Mesh::getFurthesteachAxisFromPos(const glm::vec3& pos, uint64_t vertLimit) const {
		ST_PROFILING_FUNCTION();
		if (vertLimit == 0 || vertLimit > getVerticesCount())
			vertLimit = getVerticesCount();

		uint32_t skipCount = (uint32_t)glm::ceil(getVerticesCount() / vertLimit);

		float maxX = 0;
		float maxY = 0;
		float maxZ = 0;

		for (uint32_t i = 0, j = 0; i < getVerticesCount(); i += skipCount) {
			if (j >= vertLimit)
				break;
			if (maxX < getVertices()[i].pos.x) {
				maxX = getVertices()[i].pos.x;
			}
			if (maxY < getVertices()[i].pos.y) {
				maxY = getVertices()[i].pos.y;
			}
			if (maxZ < getVertices()[i].pos.z) {
				maxZ = getVertices()[i].pos.z;
			}
			j++;
		}
		return { maxX,maxY,maxZ };
	}
	Mesh Mesh::copyAndLimit(const Ref<Mesh>& srcMesh, uint64_t vertLimit) {
		ST_PROFILING_FUNCTION();
		if (vertLimit == 0 || vertLimit > srcMesh->getVerticesCount())
			vertLimit = srcMesh->getVerticesCount();

		Ref<Mesh> mesh = createRef<Mesh>(Mesh::combine(srcMesh));

		uint32_t skipCount = (uint32_t)glm::ceil(mesh->getVerticesCount() / vertLimit);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		vertices.resize(vertLimit);
		indices.resize(vertLimit);

		for (uint32_t i = 0, j = 0; i < mesh->getVerticesCount(); i += skipCount) {
			if (j >= vertLimit)
				break;
			vertices[j] = mesh->getVertices()[i];
			indices[j] = j;
			j++;
		}
		mesh.reset();
		return Mesh(vertices, indices);
	}
	Mesh Mesh::combine(const Mesh& mesh) {
		return combine(createRef<Mesh>(mesh));
	}
	Mesh Mesh::combine(const Ref<Mesh>& mesh) {
		ST_PROFILING_FUNCTION();
		if (mesh->m_subMeshCount == 0)
			return *mesh.get();

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		uint32_t indicesOffset = 0;

		for (auto& sub : mesh->m_subMeshes) {
			indicesOffset = (uint32_t)vertices.size();
			for (auto& vertex : sub.m_vertices) {
				vertices.push_back(vertex);
			}
			for (auto& index : sub.m_indices) {
				indices.push_back(index + indicesOffset);
			}
		}
		return Mesh(vertices, indices);
	}
}