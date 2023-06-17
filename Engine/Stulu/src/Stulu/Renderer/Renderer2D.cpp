#include "st_pch.h"
#include "Renderer2D.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Math/Math.h"
#include "Stulu/Core/Application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <Stulu/Core/Resources.h>

namespace Stulu {
	struct QuadVertex {
		glm::vec3 pos;
		glm::vec2 texCoords;
		glm::vec4 color;
		float texture;
		glm::vec2 textureTiling;
	};
	struct CircleVertex {
		glm::vec3 pos;
		glm::vec3 localPos;
		glm::vec4 color;
		float thickness;
		float fade;
	};
	struct LineVertex {
		glm::vec3 pos;
		glm::vec4 color;
	};
	struct Renderer2DData {
		static const uint32_t maxQuads = 10000;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxTextureSlots = 32;


		Ref<Shader> quadShader;
		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;

		Ref<VertexArray> circleVertexArray;
		Ref<VertexBuffer> circleVertexBuffer;
		Ref<Shader> circleShader;

		Ref<VertexArray> lineVertexArray;
		Ref<VertexBuffer> lineVertexBuffer;
		Ref<Shader> lineShader;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		uint32_t circleIndexCount = 0;
		CircleVertex* circleVertexBufferBase = nullptr;
		CircleVertex* circleVertexBufferPtr = nullptr;

		uint32_t lineVertexCount = 0;
		LineVertex* lineVertexBufferBase = nullptr;
		LineVertex* lineVertexBufferPtr = nullptr;

		Ref<Texture2D> whiteTexture;
		std::array<Ref<Texture>, maxTextureSlots> textureSlots;
		uint32_t slotIndex = 1;

		glm::vec4 quadVertexPositions[4];

		glm::mat4 view;
		glm::mat4 proj;
		Ref<Camera> camera;
	};

	static Renderer2DData s_renderer2Ddata;
	void Renderer2D::init() {
		ST_PROFILING_FUNCTION();

		Ref<IndexBuffer> indexBuffer;
		{//indices
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
			indexBuffer = IndexBuffer::create(s_renderer2Ddata.maxIndices, quadIndices);
			delete[] quadIndices;
			s_renderer2Ddata.quadVertexPositions[0] = { -.5f, -.5f, .0f, 1.0f };
			s_renderer2Ddata.quadVertexPositions[1] = { .5f, -.5f, .0f, 1.0f };
			s_renderer2Ddata.quadVertexPositions[2] = { .5f,  .5f, .0f, 1.0f };
			s_renderer2Ddata.quadVertexPositions[3] = { -.5f,  .5f, .0f, 1.0f };
		}

		{//quads
			s_renderer2Ddata.quadVertexArray = VertexArray::create();
			s_renderer2Ddata.quadVertexBuffer = VertexBuffer::create(s_renderer2Ddata.maxVertices * sizeof(QuadVertex));
			s_renderer2Ddata.quadVertexBuffer->setLayout({
				{ ShaderDataType::Float3, "a_pos" },
				{ ShaderDataType::Float2, "a_texCoord" },
				{ ShaderDataType::Float4, "a_color" },
				{ ShaderDataType::Float, "a_textureIndex" },
				{ ShaderDataType::Float2, "a_textureTiling" },
				});
			s_renderer2Ddata.quadVertexArray->addVertexBuffer(s_renderer2Ddata.quadVertexBuffer);
			s_renderer2Ddata.quadVertexArray->setIndexBuffer(indexBuffer);
			s_renderer2Ddata.quadVertexBufferBase = new QuadVertex[s_renderer2Ddata.maxVertices];
			s_renderer2Ddata.quadShader = Shader::create(Application::getEngineAssetDir() + "/Shaders/2DQuad.glsl");
		}
		{//circles
			s_renderer2Ddata.circleVertexArray = VertexArray::create();
			s_renderer2Ddata.circleVertexBuffer = VertexBuffer::create(s_renderer2Ddata.maxVertices * sizeof(CircleVertex));
			s_renderer2Ddata.circleVertexBuffer->setLayout({
				{ ShaderDataType::Float3, "a_pos" },
				{ ShaderDataType::Float3, "a_localPos" },
				{ ShaderDataType::Float4, "a_color"         },
				{ ShaderDataType::Float,  "a_thickness"     },
				{ ShaderDataType::Float,  "a_fade"          },
				});
			s_renderer2Ddata.circleVertexArray->addVertexBuffer(s_renderer2Ddata.circleVertexBuffer);
			s_renderer2Ddata.circleVertexArray->setIndexBuffer(indexBuffer);
			s_renderer2Ddata.circleVertexBufferBase = new CircleVertex[s_renderer2Ddata.maxVertices];
			s_renderer2Ddata.circleShader = Shader::create(Application::getEngineAssetDir() + "/Shaders/2DCircle.glsl");
		}
		{
			s_renderer2Ddata.lineVertexArray = VertexArray::create();

			s_renderer2Ddata.lineVertexBuffer = VertexBuffer::create(s_renderer2Ddata.maxVertices * sizeof(LineVertex));
			s_renderer2Ddata.lineVertexBuffer->setLayout({
				{ ShaderDataType::Float3, "a_pos" },
				{ ShaderDataType::Float4, "a_color"    },
				});
			s_renderer2Ddata.lineVertexArray->addVertexBuffer(s_renderer2Ddata.lineVertexBuffer);
			s_renderer2Ddata.lineVertexBufferBase = new LineVertex[s_renderer2Ddata.maxVertices];
			s_renderer2Ddata.lineShader = Shader::create(Application::getEngineAssetDir() + "/Shaders/2DLine.glsl");
		}
		{//textures
			s_renderer2Ddata.whiteTexture = std::dynamic_pointer_cast<Texture2D>(Resources::getWhiteTexture());
			int samplers[s_renderer2Ddata.maxTextureSlots];
			for (int i = 0; i < s_renderer2Ddata.maxTextureSlots; i++) {
				samplers[i] = i;
				s_renderer2Ddata.whiteTexture->bind(i);//so opengl doesnt warn me for havin an sampler array with empty sampler
			}
			s_renderer2Ddata.quadShader->bind();
			s_renderer2Ddata.quadShader->setIntArray("u_textures", samplers, s_renderer2Ddata.maxTextureSlots);
			s_renderer2Ddata.textureSlots[0] = s_renderer2Ddata.whiteTexture;
		}
	}
	void Renderer2D::shutdown() {
	}
	void Renderer2D::begin() {
		ST_PROFILING_FUNCTION();
		resetQuadBatch();
		resetCircleBatch();
		resetLineBatch();

		s_renderer2Ddata.camera = nullptr;
	}
	void Renderer2D::begin(const Ref<Camera>& cam) {
		ST_PROFILING_FUNCTION();
		resetQuadBatch();
		resetCircleBatch();
		resetLineBatch();

		s_renderer2Ddata.camera = cam;
	}
	void Renderer2D::flush() {
		ST_PROFILING_FUNCTION();
		if(s_renderer2Ddata.camera)
			s_renderer2Ddata.camera->bindFrameBuffer();

		flushQuads(false);
		flushCircles(false);
		flushLines(false);

		if (s_renderer2Ddata.camera)
			s_renderer2Ddata.camera->unbindFrameBuffer();
	}

	void Renderer2D::flushQuads(bool bindCam) {
		ST_PROFILING_FUNCTION();
		if (s_renderer2Ddata.quadIndexCount > 0) {
			uint32_t dataSize = uint32_t((uint8_t*)s_renderer2Ddata.quadVertexBufferPtr - (uint8_t*)s_renderer2Ddata.quadVertexBufferBase);
			s_renderer2Ddata.quadVertexBuffer->setData(s_renderer2Ddata.quadVertexBufferBase, dataSize);
			for (uint32_t i = 0; i < s_renderer2Ddata.slotIndex; i++)
				s_renderer2Ddata.textureSlots[i]->bind(i);
			s_renderer2Ddata.quadShader->bind();
			RenderCommand::setCullMode(CullMode::BackAndFront);
			if (bindCam) {
				if (s_renderer2Ddata.camera)
					s_renderer2Ddata.camera->bindFrameBuffer();
			}
			RenderCommand::drawIndexed(s_renderer2Ddata.quadVertexArray, s_renderer2Ddata.quadIndexCount);
			if (bindCam) {
				if (s_renderer2Ddata.camera)
					s_renderer2Ddata.camera->unbindFrameBuffer();
			}
		}
	}

	void Renderer2D::flushCircles(bool bindCam) {
		ST_PROFILING_FUNCTION();
		if (s_renderer2Ddata.circleIndexCount > 0) {
			uint32_t dataSize = (uint32_t)((uint8_t*)s_renderer2Ddata.circleVertexBufferPtr - (uint8_t*)s_renderer2Ddata.circleVertexBufferBase);
			s_renderer2Ddata.circleVertexBuffer->setData(s_renderer2Ddata.circleVertexBufferBase, dataSize);
			s_renderer2Ddata.circleShader->bind();
			RenderCommand::setCullMode(CullMode::BackAndFront);
			if (bindCam) {
				if (s_renderer2Ddata.camera)
					s_renderer2Ddata.camera->bindFrameBuffer();
			}
			RenderCommand::drawIndexed(s_renderer2Ddata.circleVertexArray, s_renderer2Ddata.circleIndexCount);
			if (bindCam) {
				if (s_renderer2Ddata.camera)
					s_renderer2Ddata.camera->unbindFrameBuffer();
			}
		}
	}

	void Renderer2D::flushLines(bool bindCam) {
		ST_PROFILING_FUNCTION();
		if (s_renderer2Ddata.lineVertexCount > 0) {
			uint32_t dataSize = (uint32_t)((uint8_t*)s_renderer2Ddata.lineVertexBufferPtr - (uint8_t*)s_renderer2Ddata.lineVertexBufferBase);
			s_renderer2Ddata.lineVertexBuffer->setData(s_renderer2Ddata.lineVertexBufferBase, dataSize);
			s_renderer2Ddata.lineShader->bind();
			RenderCommand::setCullMode(CullMode::BackAndFront);
			if (bindCam) {
				if (s_renderer2Ddata.camera)
					s_renderer2Ddata.camera->bindFrameBuffer();
			}
			RenderCommand::drawLines(s_renderer2Ddata.lineVertexArray, s_renderer2Ddata.lineVertexCount);
			if (bindCam) {
				if (s_renderer2Ddata.camera)
					s_renderer2Ddata.camera->unbindFrameBuffer();
			}
		}
	}

	void Renderer2D::resetQuadBatch() {
		s_renderer2Ddata.slotIndex = 1;
		s_renderer2Ddata.quadIndexCount = 0;
		s_renderer2Ddata.quadVertexBufferPtr = s_renderer2Ddata.quadVertexBufferBase;
	}

	void Renderer2D::resetCircleBatch() {
		s_renderer2Ddata.circleIndexCount = 0;
		s_renderer2Ddata.circleVertexBufferPtr = s_renderer2Ddata.circleVertexBufferBase;
	}

	void Renderer2D::resetLineBatch() {
		s_renderer2Ddata.lineVertexCount = 0;
		s_renderer2Ddata.lineVertexBufferPtr = s_renderer2Ddata.lineVertexBufferBase;
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color) {
		if (s_renderer2Ddata.quadIndexCount >= s_renderer2Ddata.maxIndices) {
			flushQuads();
			resetQuadBatch();
		}
		glm::vec2 texCoords[] = { glm::vec2(.0f,.0f),glm::vec2(1.0f,.0f),glm::vec2(1.0f,1.0f),glm::vec2(.0f,1.0f) };

		for (int i = 0; i < 4; i++) {
			s_renderer2Ddata.quadVertexBufferPtr->pos = transform * s_renderer2Ddata.quadVertexPositions[i];
			s_renderer2Ddata.quadVertexBufferPtr->texCoords = texCoords[i];
			s_renderer2Ddata.quadVertexBufferPtr->color = color;
			s_renderer2Ddata.quadVertexBufferPtr->texture = 0.0f;
			s_renderer2Ddata.quadVertexBufferPtr->textureTiling = glm::vec2(1.0f);
			s_renderer2Ddata.quadVertexBufferPtr++;
		}
		s_renderer2Ddata.quadIndexCount += 6;
	}
	void Renderer2D::drawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color) {
		drawQuad(Math::createMat4(glm::vec3(pos, .0f), glm::vec3(size, 1.0f)), color);
	}
	void Renderer2D::drawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color) {
		drawQuad(Math::createMat4(pos, glm::vec3(size, 1.0f)), color);
	}
	void Renderer2D::drawQuad(const glm::vec3& pos, const glm::vec2& size, const float& rotation, const glm::vec4& color) {
		drawQuad(Math::createMat4(pos,glm::vec3(.0f,.0f,glm::radians(rotation)),glm::vec3(size,1.0f)), color);
	}

	void Renderer2D::drawTexturedQuad(const glm::mat4& transform, const Ref<Texture>& texture, const glm::vec2& tiling, const glm::vec4& color) {
		if (s_renderer2Ddata.quadIndexCount >= s_renderer2Ddata.maxIndices) {
			flushQuads();
			resetQuadBatch();
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
			s_renderer2Ddata.quadVertexBufferPtr->pos = transform * s_renderer2Ddata.quadVertexPositions[i];
			s_renderer2Ddata.quadVertexBufferPtr->texCoords = texCoords[i];
			s_renderer2Ddata.quadVertexBufferPtr->color = color;
			s_renderer2Ddata.quadVertexBufferPtr->texture = texureIndex;
			s_renderer2Ddata.quadVertexBufferPtr->textureTiling = tiling;
			s_renderer2Ddata.quadVertexBufferPtr++;
		}

		s_renderer2Ddata.quadIndexCount += 6;
	}
	void Renderer2D::drawTexturedQuad(const Ref<Texture2D>& texture, const glm::vec2& pos, const glm::vec2& size, const glm::vec2& tiling, const glm::vec4& color) {
		drawTexturedQuad(Math::createMat4(glm::vec3(pos, .0f), glm::vec3(size, 1.0f)),texture, texture->getSettings().tiling * tiling, color);
	}
	void Renderer2D::drawTexturedQuad(const Ref<Texture2D>& texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec2& tiling, const glm::vec4& color) {
		drawTexturedQuad(Math::createMat4(pos, glm::vec3(size, 1.0f)), texture, texture->getSettings().tiling * tiling, color);
	}
	void Renderer2D::drawTexturedQuad(const Ref<Texture2D>& texture, const glm::vec3& pos, const glm::vec2& size, const float& rotation, const glm::vec2& tiling, const glm::vec4& color) {
		drawTexturedQuad(Math::createMat4(pos, glm::vec3(.0f,.0f, glm::radians(rotation)), glm::vec3(size, 1.0f)), texture, texture->getSettings().tiling * tiling, color);
	}
	
	void Renderer2D::drawFromSpriteSheet(const glm::mat4& transform, const Ref<SubTexture2D>& sprite, const glm::vec2& tiling, const glm::vec4& color) {
		if (s_renderer2Ddata.quadIndexCount >= s_renderer2Ddata.maxIndices) {
			flushQuads();
			resetQuadBatch();
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
			s_renderer2Ddata.quadVertexBufferPtr->pos = transform * s_renderer2Ddata.quadVertexPositions[i];
			s_renderer2Ddata.quadVertexBufferPtr->texCoords = texCoords[i];
			s_renderer2Ddata.quadVertexBufferPtr->color = color;
			s_renderer2Ddata.quadVertexBufferPtr->texture = texureIndex;
			s_renderer2Ddata.quadVertexBufferPtr->textureTiling = sprite->getTexture()->getSettings().tiling * tiling;
			s_renderer2Ddata.quadVertexBufferPtr++;
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
		drawFromSpriteSheet(Math::createMat4(pos, glm::vec3(.0f, .0f, glm::radians(rotation)), glm::vec3(size, 1.0f)), sprite, tiling, color);
	}

	void Renderer2D::drawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade) {
		if (s_renderer2Ddata.circleIndexCount >= s_renderer2Ddata.maxIndices) {
			flushCircles();
			resetCircleBatch();
		}

		for (size_t i = 0; i < 4; i++)
		{
			s_renderer2Ddata.circleVertexBufferPtr->pos = transform * s_renderer2Ddata.quadVertexPositions[i];
			s_renderer2Ddata.circleVertexBufferPtr->localPos = s_renderer2Ddata.quadVertexPositions[i] * 2.0f;
			s_renderer2Ddata.circleVertexBufferPtr->color = color;
			s_renderer2Ddata.circleVertexBufferPtr->thickness = thickness;
			s_renderer2Ddata.circleVertexBufferPtr->fade = fade;
			s_renderer2Ddata.circleVertexBufferPtr++;
		}
		s_renderer2Ddata.circleIndexCount += 6;
	}
	void Renderer2D::drawCircle(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float thickness, float fade) {
		drawCircle(Math::createMat4(pos, glm::vec3(size, 1.0f)), color, thickness, fade);
	}
	void Renderer2D::drawCircle(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float thickness, float fade) {
		drawCircle(Math::createMat4(glm::vec3(pos, .0f), glm::vec3(size, 1.0f)), color, thickness, fade);
	}

	void Renderer2D::drawLine(const glm::vec3& point0, const glm::vec3& point1, const glm::vec4& color) {
		if (s_renderer2Ddata.lineVertexCount >= s_renderer2Ddata.maxVertices) {
			flushLines();
			resetLineBatch();
		}
		s_renderer2Ddata.lineVertexBufferPtr->pos = point0;
		s_renderer2Ddata.lineVertexBufferPtr->color = color;
		s_renderer2Ddata.lineVertexBufferPtr++;

		s_renderer2Ddata.lineVertexBufferPtr->pos = point1;
		s_renderer2Ddata.lineVertexBufferPtr->color = color;
		s_renderer2Ddata.lineVertexBufferPtr++;

		s_renderer2Ddata.lineVertexCount += 2;
	}
	void Renderer2D::drawLineRect(const glm::mat4& transform, const glm::vec4& color) {
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_renderer2Ddata.quadVertexPositions[i];

		drawLine(lineVertices[0], lineVertices[1], color);
		drawLine(lineVertices[1], lineVertices[2], color);
		drawLine(lineVertices[2], lineVertices[3], color);
		drawLine(lineVertices[3], lineVertices[0], color);
	}
}