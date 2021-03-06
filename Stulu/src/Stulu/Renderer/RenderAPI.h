#pragma once

#include <glm/glm.hpp>
#include "Stulu/Renderer/VertexArray.h"

namespace Stulu {
	class STULU_API RenderAPI {
	public:
		enum class API {
			none = 0,
			OpenGL = 1, Vulkan = 2,
			Direct3D11 = 3, Direct3D12 = 4,
			Metal = 5, GLES = 6
		};
		virtual ~RenderAPI() = default;


		virtual void init() = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void setDepthFunc(bool lequalLess) = 0;
		virtual void setDepthMask(bool value) = 0;
		virtual void setWireFrame(bool v) = 0;
		virtual void clear() = 0;
		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t count = 0) = 0;
		virtual void drawLines(const Ref<VertexArray>& vertexArray, const uint32_t count = 0) = 0;


		inline static API getAPI() { return s_api; };
	private:
		static API s_api;
	};
}