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

		static void drawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color);
		static void drawQuad(const Transform& transform, const glm::vec4& color);

		static void drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const Transform& transform, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });

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

#define COLOR_WHITE glm::vec4(1.0f,1.0f,1.0f,1.0f)
#define COLOR_BLACK glm::vec4(0.0f,0.0f,0.0f,1.0f)
#define COLOR_GRAY glm::vec4(0.15f,0.15f,0.15f,1.0f)

#define COLOR_BLUE glm::vec4(0.0f,0.0f,1.0f,1.0f)
#define COLOR_RED glm::vec4(1.0f,0.0f,0.0f,1.0f)
#define COLOR_GREEN glm::vec4(0.0f,1.0f,0.0f,1.0f)
#define COLOR_CYAN glm::vec4(0.0f,1.0f,1.0f,1.0f)
#define COLOR_PINK glm::vec4(1.0f,0.0f,1.0f,1.0f)
#define COLOR_YELLOW glm::vec4(1.0f,1.0f,0.0f,1.0f)

#define COLOR_PLASTIC_BLUE glm::vec4(.2f,.2f,.8f,1.0f)
#define COLOR_PLASTIC_RED glm::vec4(.8f,.2f,.2f,1.0f)
#define COLOR_PLASTIC_GREEN glm::vec4(.2f,.8f,.2f,1.0f)
#define COLOR_PLASTIC_CYAN glm::vec4(.1f,.7f,.8f,1.0f)
#define COLOR_PLASTIC_PINK glm::vec4(.8f,.1f,.8f,1.0f)
#define COLOR_PLASTIC_YELLOW glm::vec4(.8f,.8f,.1f,1.0f)