#pragma once

#include <glm/glm.hpp>
#include "Stulu/Renderer/VertexArray.h"

namespace Stulu {
	enum class StencilMode {
		DisableWriting, WriteToBuffer, BeginDrawFromBuffer, EndDrawFromBuffer
	};
	enum class CullMode {
		Back, Front, BackAndFront
	};
	class STULU_API RenderAPI {
	public:
		virtual ~RenderAPI() = default;

		virtual void init() = 0;
		virtual void setDefault() = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void setWireFrame(bool v) = 0;
		virtual void setCullMode(CullMode v) = 0;
		virtual void setStencil(StencilMode v) = 0;
		virtual void setDepthTesting(bool value) = 0;
		virtual void clear() = 0;
		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t indicesCount = 0, const uint32_t instanceCount = 0) = 0;
		virtual void drawIndexedSubMesh(const Ref<VertexArray>& vertexArray, const uint32_t indicesCount, const uint32_t indexOffset, const uint32_t vertexOffset = 0, const uint32_t instanceCount = 0) = 0;
		virtual void drawLines(const Ref<VertexArray>& vertexArray, const uint32_t count) = 0;

		virtual const glm::ivec3 getMaxComputeWorkGroupCount() const = 0;
		virtual const glm::ivec3 getMaxComputeWorkGroupSizes() const = 0;
		virtual const uint32_t getMaxComputeWorkGroupInvocationCount() const = 0;
	};
}