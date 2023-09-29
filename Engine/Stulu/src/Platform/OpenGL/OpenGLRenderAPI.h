#pragma once
#include "Stulu/Renderer/RenderAPI.h"

namespace Stulu {
	class STULU_API OpenGLRenderAPI : public RenderAPI {
	public:
		virtual void init() override;
		virtual void setDefault() override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void setWireFrame(bool v) override;
		virtual void setCullMode(CullMode v) override;
		virtual void setStencil(StencilMode v) override;
		virtual void setDepthTesting(bool v) override;
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t count = 0, const uint32_t instanceCount = 0) override;
		virtual void drawLines(const Ref<VertexArray>& vertexArray, const uint32_t counts) override;

		virtual const glm::ivec3 getMaxComputeWorkGroupCount() const override;
		virtual const glm::ivec3 getMaxComputeWorkGroupSizes() const override;
		virtual const uint32_t getMaxComputeWorkGroupInvocationCount() const override;
	private:

	};
}