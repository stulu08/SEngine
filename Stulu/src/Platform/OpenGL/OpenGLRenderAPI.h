#pragma once
#include "Stulu/Renderer/RenderAPI.h"

namespace Stulu {
	class OpenGLRenderAPI : public RenderAPI {
	public:
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;

		virtual void drawIndex(const std::shared_ptr<VertexArray>& vertexArray) override;
	private:

	};
}