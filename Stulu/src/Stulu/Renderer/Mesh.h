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
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, BufferLayout layout = {
			{ Stulu::ShaderDataType::Float3, "a_pos" },
			{ Stulu::ShaderDataType::Float3, "a_normal" },
			{ Stulu::ShaderDataType::Float2, "a_texCoord" },
			});

		const Ref<Stulu::VertexArray>& getVertexArray() { return m_vertexArray; }

	private:
		Ref<Stulu::VertexArray> m_vertexArray;

	};
}

