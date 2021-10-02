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
		inline static void setDepthFunc(bool lequalLess) {
			s_renderAPI->setDepthFunc(lequalLess);
		}
		inline static void setDepthMask(bool value) {
			s_renderAPI->setDepthMask(value);
		}
		inline static void setWireFrame(bool value) {
			s_renderAPI->setWireFrame(value);
		}
		inline static void setClearColor(const glm::vec4& color) {
			s_renderAPI->setClearColor(color);
		}
		inline static void clear() {
			s_renderAPI->clear();
		}
		inline static void drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t count = 0) {
			ST_PROFILING_RENDERDATA_ADDDRAWCALL();
			s_renderAPI->drawIndexed(vertexArray, count);
		}
	private:
		static Scope<RenderAPI> s_renderAPI;
	};
}