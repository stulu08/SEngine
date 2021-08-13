#pragma once
#include "Stulu/Renderer/RenderAPI.h"

namespace Stulu {
	class OpenGLRenderAPI : public RenderAPI {
	public:
		virtual void init() override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void setDepthFunc(bool lequalLess) override;
		virtual void setDepthMask(bool value) override;
		virtual void setWireFrame(bool v) override;
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t count = 0) override;
		virtual void drawPrimitiveArray(const uint32_t count) override;
	private:

	};
}