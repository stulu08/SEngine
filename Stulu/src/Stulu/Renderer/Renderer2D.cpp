#include "st_pch.h"
#include "Renderer2D.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Math/Math.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Stulu {
	struct QuadVertex {
		glm::vec3 pos;
		glm::vec2 texCoords;
		glm::vec4 color;
		glm::int32 texture;
	};
	struct Renderer2DData {
		const uint32_t maxQuads = 10000;
		const uint32_t maxIndices = maxQuads * 6;
		const uint32_t maxVertices = maxQuads * 4;
		Ref<Shader> m_shader;
		Ref<Texture2D> m_texture;
		Ref<VertexArray> m_quadVertexArray;
		Ref<VertexBuffer> m_quadVertexBuffer;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;
	};

	static Renderer2DData s_renderer2Ddata;
	void Renderer2D::init() {
		ST_PROFILING_FUNCTION();
		s_renderer2Ddata.m_quadVertexArray = VertexArray::create();

		s_renderer2Ddata.m_quadVertexBuffer = VertexBuffer::create(s_renderer2Ddata.maxVertices * sizeof(QuadVertex));

		s_renderer2Ddata.m_quadVertexBuffer->setLayout({
			{ ShaderDataType::Float3, "a_pos" },
			{ ShaderDataType::Float2, "a_texCoord" },
			{ ShaderDataType::Float4, "a_color" },
			{ ShaderDataType::Int, "a_texture" },
		});
		s_renderer2Ddata.m_quadVertexArray->addVertexBuffer(s_renderer2Ddata.m_quadVertexBuffer);

		s_renderer2Ddata.quadVertexBufferBase = new QuadVertex[s_renderer2Ddata.maxVertices];

		uint32_t* quadIndices = new uint32_t[s_renderer2Ddata.maxIndices];
		uint32_t offset = 0;
		for(uint32_t i = 0; i < s_renderer2Ddata.maxIndices; i+=6){
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
			offset += 4;
		}

		Ref<IndexBuffer> indexBuffer = IndexBuffer::create(s_renderer2Ddata.maxIndices, quadIndices);
		s_renderer2Ddata.m_quadVertexArray->setIndexBuffer(indexBuffer);
		delete[] quadIndices;

		s_renderer2Ddata.m_texture = Texture2D::create("Stulu/assets/Textures/quad.png");
		s_renderer2Ddata.m_shader = Shader::create("Stulu/assets/Shaders/default2D.glsl");
		s_renderer2Ddata.m_shader->bind();
		s_renderer2Ddata.m_shader->setInt("u_texture", 1);

	}
	void Renderer2D::shutdown() {
		ST_PROFILING_FUNCTION();
	}
	void Renderer2D::beginScene(const Camera& cam) {
		ST_PROFILING_FUNCTION();
		s_renderer2Ddata.m_shader->bind();
		s_renderer2Ddata.m_shader->setMat4("u_viewProjection", cam.getViewProjectionMatrix());
		s_renderer2Ddata.quadIndexCount = 0;
		s_renderer2Ddata.quadVertexBufferPtr = s_renderer2Ddata.quadVertexBufferBase;

	}
	void Renderer2D::endScene() {
		ST_PROFILING_FUNCTION();
		s_renderer2Ddata.m_texture->bind(1);
		uint32_t dataSize = (uint32_t)((uint8_t*)s_renderer2Ddata.quadVertexBufferPtr - (uint8_t*)s_renderer2Ddata.quadVertexBufferBase);
		s_renderer2Ddata.m_quadVertexBuffer->setData(s_renderer2Ddata.quadVertexBufferBase, dataSize);
		flush();
	}
	void Renderer2D::flush() {
		ST_PROFILING_FUNCTION(); 
		s_renderer2Ddata.m_quadVertexArray->bind();
		RenderCommand::drawIndexed(s_renderer2Ddata.m_quadVertexArray, s_renderer2Ddata.quadIndexCount);
	}
	void Renderer2D::drawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color) {
		drawQuad(glm::vec3(pos, 0.0f), size, color);
	}
	void Renderer2D::drawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color) {
		ST_PROFILING_FUNCTION();
		//glm::vec3 bottomLeftPos = { pos.x - size.x / 2, pos.y - size.y / 2, pos.z };
		glm::vec3 bottomLeftPos = pos;
		s_renderer2Ddata.quadVertexBufferPtr->pos = bottomLeftPos;
		s_renderer2Ddata.quadVertexBufferPtr->texCoords = {0.0f,0.0f};
		s_renderer2Ddata.quadVertexBufferPtr->color = color;
		s_renderer2Ddata.quadVertexBufferPtr->texture = 1;
		s_renderer2Ddata.quadVertexBufferPtr++;

		s_renderer2Ddata.quadVertexBufferPtr->pos = { bottomLeftPos.x + size.x, bottomLeftPos.y, bottomLeftPos.z };
		s_renderer2Ddata.quadVertexBufferPtr->texCoords = { 1.0f,0.0f };
		s_renderer2Ddata.quadVertexBufferPtr->color = color;
		s_renderer2Ddata.quadVertexBufferPtr->texture = 1;
		s_renderer2Ddata.quadVertexBufferPtr++;

		s_renderer2Ddata.quadVertexBufferPtr->pos = { bottomLeftPos.x + size.x, bottomLeftPos.y + size.y, bottomLeftPos.z };
		s_renderer2Ddata.quadVertexBufferPtr->texCoords = { 1.0f,1.0f };
		s_renderer2Ddata.quadVertexBufferPtr->color = color;
		s_renderer2Ddata.quadVertexBufferPtr->texture = 1;
		s_renderer2Ddata.quadVertexBufferPtr++;

		s_renderer2Ddata.quadVertexBufferPtr->pos = { bottomLeftPos.x, bottomLeftPos.y + size.y, bottomLeftPos.z };
		s_renderer2Ddata.quadVertexBufferPtr->texCoords = { 0.0f,1.0f };
		s_renderer2Ddata.quadVertexBufferPtr->color = color;
		s_renderer2Ddata.quadVertexBufferPtr->texture = 1;
		s_renderer2Ddata.quadVertexBufferPtr++;

		s_renderer2Ddata.quadIndexCount += 6;
	}
	void Renderer2D::drawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color) {

		ST_PROFILING_FUNCTION();
		//glm::vec3 bottomLeftPos = { pos.x - size.x / 2, pos.y - size.y / 2, pos.z };
		glm::vec3 bottomLeftPos = pos;
		s_renderer2Ddata.quadVertexBufferPtr->pos = glm::rotate(glm::quat(glm::radians(rotation)), bottomLeftPos);
		s_renderer2Ddata.quadVertexBufferPtr->texCoords = { 0.0f,0.0f };
		s_renderer2Ddata.quadVertexBufferPtr->color = color;
		s_renderer2Ddata.quadVertexBufferPtr++;

		s_renderer2Ddata.quadVertexBufferPtr->pos = glm::rotate(glm::quat(glm::radians(rotation)), {bottomLeftPos.x + size.x, bottomLeftPos.y, bottomLeftPos.z});
		s_renderer2Ddata.quadVertexBufferPtr->texCoords = { 1.0f,0.0f };
		s_renderer2Ddata.quadVertexBufferPtr->color = color;
		s_renderer2Ddata.quadVertexBufferPtr++;

		s_renderer2Ddata.quadVertexBufferPtr->pos = glm::rotate(glm::quat(glm::radians(rotation)), { bottomLeftPos.x + size.x, bottomLeftPos.y + size.y, bottomLeftPos.z });
		s_renderer2Ddata.quadVertexBufferPtr->texCoords = { 1.0f,1.0f };
		s_renderer2Ddata.quadVertexBufferPtr->color = color;
		s_renderer2Ddata.quadVertexBufferPtr++;

		s_renderer2Ddata.quadVertexBufferPtr->pos = glm::rotate(glm::quat(glm::radians(rotation)), { bottomLeftPos.x, bottomLeftPos.y + size.y, bottomLeftPos.z });
		s_renderer2Ddata.quadVertexBufferPtr->texCoords = { 0.0f,1.0f };
		s_renderer2Ddata.quadVertexBufferPtr->color = color;
		s_renderer2Ddata.quadVertexBufferPtr++;

		s_renderer2Ddata.quadIndexCount += 6;
	}
	void Renderer2D::drawQuad(const Transform& transform, const glm::vec4& color) {
		if(transform.rotation != glm::vec3(0.0f,0.0f,0.0f))
			drawQuad(transform.position, glm::vec2(transform.scale.x, transform.scale.y), transform.rotation, color);
		else
			drawQuad(transform.position, glm::vec2(transform.scale.x, transform.scale.y), color);
	}

	void Renderer2D::drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color) {
		ST_PROFILING_FUNCTION();
		s_renderer2Ddata.m_shader->setMat4("u_transform",
			glm::translate(glm::mat4(1.0f), pos)
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f))
		);
		s_renderer2Ddata.m_shader->setFloat2("u_textureTiling", texture->tiling);
		s_renderer2Ddata.m_shader->setFloat4("u_color", color);
		texture->bind();

		s_renderer2Ddata.m_quadVertexArray->bind();
		RenderCommand::drawIndexed(s_renderer2Ddata.m_quadVertexArray);
	}
	void Renderer2D::drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color) {
		ST_PROFILING_FUNCTION();
		s_renderer2Ddata.m_shader->setMat4("u_transform",
			glm::translate(glm::mat4(1.0f), pos)
			* glm::toMat4(glm::quat(glm::radians(rotation)))
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f))
		);
		s_renderer2Ddata.m_shader->setFloat2("u_textureTiling", texture->tiling);
		s_renderer2Ddata.m_shader->setFloat4("u_color", color);
		texture->bind();

		s_renderer2Ddata.m_quadVertexArray->bind();
		RenderCommand::drawIndexed(s_renderer2Ddata.m_quadVertexArray);
	}
	void Renderer2D::drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const Transform& transform, const glm::vec4& color) {
		ST_PROFILING_FUNCTION();
		s_renderer2Ddata.m_shader->setMat4("u_transform", transform);
		s_renderer2Ddata.m_shader->setFloat2("u_textureTiling", texture->tiling);
		s_renderer2Ddata.m_shader->setFloat4("u_color", color);
		texture->bind();

		s_renderer2Ddata.m_quadVertexArray->bind();
		RenderCommand::drawIndexed(s_renderer2Ddata.m_quadVertexArray);
	}

}
