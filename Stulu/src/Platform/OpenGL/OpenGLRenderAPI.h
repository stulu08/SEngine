#pragma once
#include "Stulu/Renderer/RenderAPI.h"

namespace Stulu {
	class OpenGLRenderAPI : public RenderAPI {
	public:
		virtual void init() override;
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;

		virtual void drawIndex(const Ref<VertexArray>& vertexArray) override;
	private:

	};
}