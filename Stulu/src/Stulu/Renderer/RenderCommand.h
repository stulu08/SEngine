#pragma once
#include "Stulu/Renderer/RenderAPI.h"

namespace Stulu{
	class RenderCommand {
	public:
		inline static void init() {
			s_renderAPI->init();
		}
		inline static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_renderAPI->setViewport(x, y, width, height);
		}
		inline static void setClearColor(const glm::vec4& color) {
			s_renderAPI->setClearColor(color);
		}
		inline static void clear() {
			s_renderAPI->clear();
		}
		inline static void drawIndex(const Ref<VertexArray>& vertexArray) {
			s_renderAPI->drawIndex(vertexArray);
		}
	private:
		static Scope<RenderAPI> s_renderAPI;
	};
}