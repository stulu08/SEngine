#pragma once
#include "Stulu/Renderer/Buffer.h"
#include "Stulu/Renderer/VertexArray.h"
#include "glm/glm.hpp"

namespace Stulu {
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};
	class Mesh {
	public:
		Mesh(){}
		Mesh(std::vector<float> vertexArray, std::vector<uint32_t> indices, BufferLayout layout = {
			{ Stulu::ShaderDataType::Float3, "a_pos" },
			{ Stulu::ShaderDataType::Float3, "a_normal" },
			{ Stulu::ShaderDataType::Float2, "a_texCoord" },
			});
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

		void recalculate();

		void setVertices(std::vector<Vertex>& vertices) { m_vertices = vertices; }
		std::vector<Vertex>& getVertices() { return m_vertices; }

		void setIndices(std::vector<uint32_t>& indices) { m_indices = indices; }
		std::vector<uint32_t>& getIndices() { return m_indices; }

		static BufferLayout getDefaultLayout() { return m_defaultLayout; }
		const Ref<Stulu::VertexArray>& getVertexArray() { return m_vertexArray; }
	private:
		Ref<Stulu::VertexArray> m_vertexArray;
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

		static BufferLayout m_defaultLayout;
	};
}

