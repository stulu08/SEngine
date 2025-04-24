#pragma once

#include "Stulu/Renderer/SubTexture2D.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Renderer/Camera.h"

namespace Stulu {
	class STULU_API Renderer2D {
	public:
		static void init();
		static void shutdown();
		//no rendering
		static void begin();
		static void begin(const Ref<Camera>& cam);
		static void flush();

		static void drawQuad(const glm::mat4& transform, const glm::vec4& color);
		static void drawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& pos, const glm::vec2& size, const float& rotation, const glm::vec4& color);

		static void drawTexturedQuad(const glm::mat4& transform, Texture2D* texture, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTexturedQuad(Texture2D* texture, const glm::vec2& pos, const glm::vec2& size, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTexturedQuad(Texture2D* texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawTexturedQuad(Texture2D* texture, const glm::vec3& pos, const glm::vec2& size, const float& rotation, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });

		static void drawFromSpriteSheet(const glm::mat4& transform, const Ref<SubTexture2D>& sprite, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawFromSpriteSheet(const Ref<SubTexture2D>& sprite, const glm::vec2& pos, const glm::vec2& size, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawFromSpriteSheet(const Ref<SubTexture2D>& sprite, const glm::vec3& pos, const glm::vec2& size, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawFromSpriteSheet(const Ref<SubTexture2D>& sprite, const glm::vec3& pos, const glm::vec2& size, const float& rotation, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		
		static void drawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f);
		static void drawCircle(const glm::vec3& pos, const glm::vec2& size = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f }, float thickness = 1.0f, float fade = 0.005f);
		static void drawCircle(const glm::vec2& pos, const glm::vec2& size = glm::vec2(1.0f), const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f }, float thickness = 1.0f, float fade = 0.005f);

		static void drawLine(const glm::vec3& point0, const glm::vec3& point1, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		static void drawLine(const glm::vec3& point0, const glm::vec3& point1, const glm::vec4& color1, const glm::vec4& color2);
		static void drawLineRect(const glm::mat4& transform, const glm::vec4& color);

		static void drawSlider(const glm::vec3& pos, const glm::vec3& size, float progress = 0.5f, const glm::vec4& color = { 1.0f,1.0f,1.0f,1.0f }, const glm::vec4& fillColor = { 1.0f,1.0f,1.0f,1.0f });
	private:
		static void flushQuads(bool bindCam = true);
		static void flushCircles(bool bindCam = true);
		static void flushLines(bool bindCam = true);

		static void resetQuadBatch();
		static void resetCircleBatch();
		static void resetLineBatch();

		friend class Scene;
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