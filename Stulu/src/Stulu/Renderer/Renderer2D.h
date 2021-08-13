#pragma once

#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/Texture.h"
#include "Stulu/Renderer/Transform.h"

namespace Stulu {
	class Renderer2D {
	public:
		static void init();
		static void shutdown();
		static void beginScene(const Camera& cam);
		static void endScene();
		static void flush();

		static void drawQuad(const glm::vec3& pos, const glm::vec2& size = { 1.0f,1.0f }, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawQuad(const glm::vec3& pos, const glm::vec2& size = { 1.0f,1.0f }, const glm::vec3& rotation = { 0.0f,0.0f,0.0f }, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawQuad(const glm::mat4& transform, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });

		static void drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const glm::vec3& pos, const glm::vec2& size = { 1.0f,1.0f }, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const glm::vec3& pos, const glm::vec2& size = { 1.0f,1.0f }, const glm::vec3& rotation = { 0.0f,0.0f,0.0f }, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const glm::mat4& transform, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });

	private:
	};
}
#define COLOR_WHITE_VEC4 {1.0f,1.0f,1.0f,1.0f}
#define COLOR_BLACK_VEC4 {0.0f,0.0f,0.0f,1.0f}
#define COLOR_GRAY_VEC4 {0.15f,0.15f,0.15f,1.0f}

#define COLOR_BLUE_VEC4 {0.0f,0.0f,1.0f,1.0f}
#define COLOR_RED_VEC4 {1.0f,0.0f,0.0f,1.0f}
#define COLOR_GREEN_VEC4 {0.0f,1.0f,0.0f,1.0f}
#define COLOR_CYAN_VEC4 {0.0f,1.0f,1.0f,1.0f}
#define COLOR_PINK_VEC4 {1.0f,0.0f,1.0f,1.0f}
#define COLOR_YELLOW_VEC4 {1.0f,1.0f,0.0f,1.0f}

#define COLOR_PLASTIC_BLUE_VEC4 {.2f,.2f,.8f,1.0f}
#define COLOR_PLASTIC_RED_VEC4 {.8f,.2f,.2f,1.0f}
#define COLOR_PLASTIC_GREEN_VEC4 {.2f,.8f,.2f,1.0f}
#define COLOR_PLASTIC_CYAN_VEC4 {.1f,.7f,.8f,1.0f}
#define COLOR_PLASTIC_PINK_VEC4 {.8f,.1f,.8f,1.0f}
#define COLOR_PLASTIC_YELLOW_VEC4 {.8f,.8f,.1f,1.0f}