#pragma once

#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/SubTexture2D.h"

namespace Stulu {
	class Renderer2D {
	public:
		static void init();
		static void shutdown();
		static void beginScene(const Camera& cam);
		static void endScene();

		static void drawQuad(const glm::mat4& transform, const glm::vec4& color);
		static void drawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& pos, const glm::vec2& size, const float& rotation, const glm::vec4& color);

		static void drawTexturedQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTexturedQuad(const Ref<Texture2D>& texture, const glm::vec2& pos, const glm::vec2& size, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTexturedQuad(const Ref<Texture2D>& texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTexturedQuad(const Ref<Texture2D>& texture, const glm::vec3& pos, const glm::vec2& size, const float& rotation, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });


		static void drawFromSpriteSheet(const glm::mat4& transform, const Ref<SubTexture2D>& sprite, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawFromSpriteSheet(const Ref<SubTexture2D>& sprite, const glm::vec2& pos, const glm::vec2& size, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawFromSpriteSheet(const Ref<SubTexture2D>& sprite, const glm::vec3& pos, const glm::vec2& size, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawFromSpriteSheet(const Ref<SubTexture2D>& sprite, const glm::vec3& pos, const glm::vec2& size, const float& rotation, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });

	private:
		static void flush();
		static void FlushReset();
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
#define COLOR_ORANGE_VEC4 {1.0f,0.647,0.0f,1.0f}

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
#define COLOR_ORANGE glm::vec4(1.0f,0.647,0.0f,1.0f)

#define COLOR_PLASTIC_BLUE glm::vec4(.2f,.2f,.8f,1.0f)
#define COLOR_PLASTIC_RED glm::vec4(.8f,.2f,.2f,1.0f)
#define COLOR_PLASTIC_GREEN glm::vec4(.2f,.8f,.2f,1.0f)
#define COLOR_PLASTIC_CYAN glm::vec4(.1f,.7f,.8f,1.0f)
#define COLOR_PLASTIC_PINK glm::vec4(.8f,.1f,.8f,1.0f)
#define COLOR_PLASTIC_YELLOW glm::vec4(.8f,.8f,.1f,1.0f)