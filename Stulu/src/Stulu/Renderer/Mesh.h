#pragma once
#include "Stulu/Renderer/Buffer.h"
#include "Stulu/Renderer/VertexArray.h"
#include "glm/glm.hpp"

namespace Stulu {
	struct vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoords;
		vertex(float posX, float posY, float posZ, float normalX, float normalY, float normalZ, float texCoordX, float texCoordY)
			: pos(posX,posY,posZ),normal(normalX,normalY,normalZ),texCoords(texCoordX,texCoordY){}
		vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texCoords)
			:pos(pos),normal(normal),texCoords(texCoords){}
		vertex()
			:pos(.0f),normal(.0f),texCoords(.0f){}
	};
	class Mesh {
	public:
		Mesh(){}
		Mesh(std::vector<float> vertexArray, std::vector<uint32_t> indices, BufferLayout layout = {
			{ Stulu::ShaderDataType::Float3, "a_pos" },
			{ Stulu::ShaderDataType::Float3, "a_normal" },
			{ Stulu::ShaderDataType::Float2, "a_texCoord" },
			});
		Mesh(std::vector<vertex> vertices, std::vector<uint32_t> indices, BufferLayout layout = {
			{ Stulu::ShaderDataType::Float3, "a_pos" },
			{ Stulu::ShaderDataType::Float3, "a_normal" },
			{ Stulu::ShaderDataType::Float2, "a_texCoord" },
			});

		const Ref<Stulu::VertexArray>& getVertexArray() { return m_vertexArray; }

	private:
		Ref<Stulu::VertexArray> m_vertexArray;

	};
}

