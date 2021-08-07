#pragma once

#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/Texture.h"
#include "Stulu/Math/Transform.h"

namespace Stulu {
	class Renderer2D {
	public:
		static void init();
		static void shutdown();
		static void beginScene(const OrthographicCamera& cam);
		static void endScene();

		static void drawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec3& rotation = { 0.0f,0.0f,0.0f }, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec3& rotation = { 0.0f,0.0f,0.0f }, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawQuad(Transform& transform, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });

		static void drawTexture2D(const Stulu::Ref<Stulu::Texture2D> texture, const glm::vec2& pos, const glm::vec2& size, const glm::vec3& rotation = { 0.0f,0.0f,0.0f });
		static void drawTexture2D(const Stulu::Ref<Stulu::Texture2D> texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec3& rotation = { 0.0f,0.0f,0.0f });
		static void drawTexture2D(const Stulu::Ref<Stulu::Texture2D> texture, Transform& transform);

		static void drawTriangle(const glm::vec2& pos, const glm::vec2& size, const glm::vec3& rotation = { 0.0f,0.0f,0.0f }, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTriangle(const glm::vec3& pos, const glm::vec2& size, const glm::vec3& rotation = { 0.0f,0.0f,0.0f }, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTriangle(Transform& transform, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
	private:
	};
}