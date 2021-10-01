#include "st_pch.h"
#include "Renderer2D.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Math/Math.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Stulu {
	struct QuadVertex {
		glm::vec3 pos;
		glm::vec2 texCoords;
		glm::vec4 color;
		float texture;
		glm::vec2 textureTiling;
	};
	struct Renderer2DData {
		static const uint32_t maxQuads = 10000;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxTextureSlots = 32;
		Ref<Shader> shader;
		Ref<Texture2D> whiteTexture;
		Ref<VertexArray> vertexArray;
		Ref<VertexBuffer> vertexBuffer;

		uint32_t quadIndexCount = 0;
		QuadVertex* vertexBufferBase = nullptr;
		QuadVertex* vertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t slotIndex = 1;

		glm::vec4 quadVertexPositions[4];
	};

	static Renderer2DData s_renderer2Ddata;
	void Renderer2D::init() {
		ST_PROFILING_FUNCTION();
		s_renderer2Ddata.vertexArray = VertexArray::create();

		s_renderer2Ddata.vertexBuffer = VertexBuffer::create(s_renderer2Ddata.maxVertices * sizeof(QuadVertex));

		s_renderer2Ddata.vertexBuffer->setLayout({
			{ ShaderDataType::Float3, "a_pos" },
			{ ShaderDataType::Float2, "a_texCoord" },
			{ ShaderDataType::Float4, "a_color" },
			{ ShaderDataType::Float, "a_textureIndex" },
			{ ShaderDataType::Float2, "a_textureTiling" },
			});
		s_renderer2Ddata.vertexArray->addVertexBuffer(s_renderer2Ddata.vertexBuffer);

		s_renderer2Ddata.vertexBufferBase = new QuadVertex[s_renderer2Ddata.maxVertices];

		uint32_t* quadIndices = new uint32_t[s_renderer2Ddata.maxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_renderer2Ddata.maxIndices; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
			offset += 4;
		}

		Ref<IndexBuffer> indexBuffer = IndexBuffer::create(s_renderer2Ddata.maxIndices, quadIndices);
		s_renderer2Ddata.vertexArray->setIndexBuffer(indexBuffer);
		delete[] quadIndices;

		s_renderer2Ddata.whiteTexture = Texture2D::create(1,1);
		uint32_t datawhite = 0xffffffff;
		s_renderer2Ddata.whiteTexture->setData(&datawhite, sizeof(uint32_t));
		s_renderer2Ddata.shader = Shader::create("Stulu/assets/Shaders/default2D.glsl");
		int samplers[s_renderer2Ddata.maxTextureSlots];
		for (int i = 0; i < s_renderer2Ddata.maxTextureSlots; i++) {
			samplers[i] = i;
		}
		s_renderer2Ddata.shader->bind();
		s_renderer2Ddata.shader->setIntArray("u_textures", samplers, s_renderer2Ddata.maxTextureSlots);

		s_renderer2Ddata.textureSlots[0] = s_renderer2Ddata.whiteTexture;
		s_renderer2Ddata.quadVertexPositions[0] = { -.5f, -.5f, .0f, 1.0f };
		s_renderer2Ddata.quadVertexPositions[1] = {  .5f, -.5f, .0f, 1.0f };
		s_renderer2Ddata.quadVertexPositions[2] = {  .5f,  .5f, .0f, 1.0f };
		s_renderer2Ddata.quadVertexPositions[3] = { -.5f,  .5f, .0f, 1.0f };
	}
	void Renderer2D::shutdown() {
		ST_PROFILING_FUNCTION();
	}
	void Renderer2D::beginScene() {
		ST_PROFILING_FUNCTION();
		s_renderer2Ddata.quadIndexCount = 0;
		s_renderer2Ddata.slotIndex = 1;
		s_renderer2Ddata.vertexBufferPtr = s_renderer2Ddata.vertexBufferBase;
	}
	void Renderer2D::beginScene(const Ref<Camera>& cam, const TransformComponent& transform) {
		ST_PROFILING_FUNCTION();
		Renderer::beginScene(cam, transform);
		s_renderer2Ddata.quadIndexCount = 0;
		s_renderer2Ddata.slotIndex = 1;
		s_renderer2Ddata.vertexBufferPtr = s_renderer2Ddata.vertexBufferBase;

	}
	void Renderer2D::endScene() {
		ST_PROFILING_FUNCTION();

		flush();
		Renderer::endScene();
	}
	void Renderer2D::flush() {
		ST_PROFILING_FUNCTION(); 
		if (s_renderer2Ddata.quadIndexCount <= 0) {
			return;
		}
		uint32_t dataSize = uint32_t((uint8_t*)s_renderer2Ddata.vertexBufferPtr - (uint8_t*)s_renderer2Ddata.vertexBufferBase);
		s_renderer2Ddata.vertexBuffer->setData(s_renderer2Ddata.vertexBufferBase, dataSize);

		for (uint32_t i = 0; i < s_renderer2Ddata.slotIndex; i++)
			s_renderer2Ddata.textureSlots[i]->bind(i);

		s_renderer2Ddata.shader->bind();
		s_renderer2Ddata.vertexArray->bind();
		RenderCommand::drawIndexed(s_renderer2Ddata.vertexArray, s_renderer2Ddata.quadIndexCount);
	}
	void Renderer2D::FlushReset() {
		flush();
		s_renderer2Ddata.quadIndexCount = 0;
		s_renderer2Ddata.slotIndex = 1;
		s_renderer2Ddata.vertexBufferPtr = s_renderer2Ddata.vertexBufferBase;
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color) {
		ST_PROFILING_FUNCTION();
		if (s_renderer2Ddata.quadIndexCount >= s_renderer2Ddata.maxIndices) {
			FlushReset();
		}
		glm::vec2 texCoords[] = { glm::vec2(.0f,.0f),glm::vec2(1.0f,.0f),glm::vec2(1.0f,1.0f),glm::vec2(.0f,1.0f) };

		for (int i = 0; i < 4; i++) {
			s_renderer2Ddata.vertexBufferPtr->pos = transform * s_renderer2Ddata.quadVertexPositions[i];
			s_renderer2Ddata.vertexBufferPtr->texCoords = texCoords[i];
			s_renderer2Ddata.vertexBufferPtr->color = color;
			s_renderer2Ddata.vertexBufferPtr->texture = 0.0f;
			s_renderer2Ddata.vertexBufferPtr->textureTiling = glm::vec2(1.0f);
			s_renderer2Ddata.vertexBufferPtr++;
		}
		s_renderer2Ddata.quadIndexCount += 6;
	}
	void Renderer2D::drawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color) {
		drawQuad(Math::createMat4(glm::vec3(pos, .0f), glm::vec3(.0f), glm::vec3(size, 1.0f)), color);
	}
	void Renderer2D::drawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color) {
		drawQuad(Math::createMat4(pos, glm::vec3(.0f), glm::vec3(size, 1.0f)), color);
	}
	void Renderer2D::drawQuad(const glm::vec3& pos, const glm::vec2& size, const float& rotation, const glm::vec4& color) {
		drawQuad(Math::createMat4(pos,glm::vec3(.0f,.0f,rotation),glm::vec3(size,1.0f)), color);
	}

	void Renderer2D::drawTexturedQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec2& tiling, const glm::vec4& color) {
		ST_PROFILING_FUNCTION();
		if (s_renderer2Ddata.quadIndexCount >= s_renderer2Ddata.maxIndices) {
			FlushReset();
		}
		float texureIndex = 0.0f;
		for (uint32_t i = 1; i < s_renderer2Ddata.slotIndex; i++) {
			if (*s_renderer2Ddata.textureSlots[i].get() == *texture.get()) {
				texureIndex = (float)i;
				break;
			}
		}

		if (texureIndex == 0.0f) {
			texureIndex = (float)s_renderer2Ddata.slotIndex;
			s_renderer2Ddata.textureSlots[s_renderer2Ddata.slotIndex] = texture;
			s_renderer2Ddata.slotIndex++;
		}
		glm::vec2 texCoords[] = { glm::vec2(.0f,.0f),glm::vec2(1.0f,.0f),glm::vec2(1.0f,1.0f),glm::vec2(.0f,1.0f) };
		for (int i = 0; i < 4; i++) {
			s_renderer2Ddata.vertexBufferPtr->pos = transform * s_renderer2Ddata.quadVertexPositions[i];
			s_renderer2Ddata.vertexBufferPtr->texCoords = texCoords[i];
			s_renderer2Ddata.vertexBufferPtr->color = color;
			s_renderer2Ddata.vertexBufferPtr->texture = texureIndex;
			s_renderer2Ddata.vertexBufferPtr->textureTiling = texture->tiling * tiling;
			s_renderer2Ddata.vertexBufferPtr++;
		}

		s_renderer2Ddata.quadIndexCount += 6;
	}
	void Renderer2D::drawTexturedQuad(const Ref<Texture2D>& texture, const glm::vec2& pos, const glm::vec2& size, const glm::vec2& tiling, const glm::vec4& color) {
		drawTexturedQuad(Math::createMat4(glm::vec3(pos, .0f), glm::vec3(.0f), glm::vec3(size, 1.0f)),texture, tiling, color);
	}
	void Renderer2D::drawTexturedQuad(const Ref<Texture2D>& texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec2& tiling, const glm::vec4& color) {
		drawTexturedQuad(Math::createMat4(pos, glm::vec3(.0f), glm::vec3(size, 1.0f)), texture, tiling, color);
	}
	void Renderer2D::drawTexturedQuad(const Ref<Texture2D>& texture, const glm::vec3& pos, const glm::vec2& size, const float& rotation, const glm::vec2& tiling, const glm::vec4& color) {
		drawTexturedQuad(Math::createMat4(pos, glm::vec3(.0f,.0f,rotation), glm::vec3(size, 1.0f)), texture, tiling, color);
	}
	void Renderer2D::drawFromSpriteSheet(const glm::mat4& transform, const Ref<SubTexture2D>& sprite, const glm::vec2& tiling, const glm::vec4& color) {
		ST_PROFILING_FUNCTION();
		if (s_renderer2Ddata.quadIndexCount >= s_renderer2Ddata.maxIndices) {
			FlushReset();
		}
		float texureIndex = 0.0f;
		for (uint32_t i = 1; i < s_renderer2Ddata.slotIndex; i++) {
			if (*s_renderer2Ddata.textureSlots[i].get() == *sprite->getTexture()) {
				texureIndex = (float)i;
				break;
			}
		}

		if (texureIndex == 0.0f) {
			texureIndex = (float)s_renderer2Ddata.slotIndex;
			s_renderer2Ddata.textureSlots[s_renderer2Ddata.slotIndex] = sprite->getTexture();
			s_renderer2Ddata.slotIndex++;
		}
		const glm::vec2* texCoords = sprite->getTexCoords();
		for (int i = 0; i < 4; i++) {
			s_renderer2Ddata.vertexBufferPtr->pos = transform * s_renderer2Ddata.quadVertexPositions[i];
			s_renderer2Ddata.vertexBufferPtr->texCoords = texCoords[i];
			s_renderer2Ddata.vertexBufferPtr->color = color;
			s_renderer2Ddata.vertexBufferPtr->texture = texureIndex;
			s_renderer2Ddata.vertexBufferPtr->textureTiling = sprite->getTexture()->tiling * tiling;
			s_renderer2Ddata.vertexBufferPtr++;
		}

		s_renderer2Ddata.quadIndexCount += 6;
	}
	void Renderer2D::drawFromSpriteSheet(const Ref<SubTexture2D>& sprite, const glm::vec2& pos, const glm::vec2& size, const glm::vec2& tiling, const glm::vec4& color) {
		drawFromSpriteSheet(Math::createMat4(glm::vec3(pos,.0f), glm::vec3(.0f), glm::vec3(size, 1.0f)), sprite, tiling, color);
	}
	void Renderer2D::drawFromSpriteSheet(const Ref<SubTexture2D>& sprite, const glm::vec3& pos, const glm::vec2& size, const glm::vec2& tiling, const glm::vec4& color) {
		drawFromSpriteSheet(Math::createMat4(pos, glm::vec3(.0f), glm::vec3(size, 1.0f)), sprite, tiling, color);
	}
	void Renderer2D::drawFromSpriteSheet(const Ref<SubTexture2D>& sprite, const glm::vec3& pos, const glm::vec2& size, const float& rotation, const glm::vec2& tiling, const glm::vec4& color) {
		drawFromSpriteSheet(Math::createMat4(pos, glm::vec3(.0f, .0f, rotation), glm::vec3(size, 1.0f)), sprite, tiling, color);
	}

}
