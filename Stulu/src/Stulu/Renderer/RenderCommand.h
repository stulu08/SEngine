#pragma once
#include "Stulu/Renderer/RenderAPI.h"

namespace Stulu{
	class RenderCommand {
	public:
		inline static void setClearColor(const glm::vec4& color) {
			s_renderAPI->setClearColor(color);
		}
		inline static void clear() {
			s_renderAPI->clear();
		}
		inline static void drawIndex(const std::shared_ptr<VertexArray>& vertexArray) {
			s_renderAPI->drawIndex(vertexArray);
		}
	private:
		static RenderAPI* s_renderAPI;
	};
}