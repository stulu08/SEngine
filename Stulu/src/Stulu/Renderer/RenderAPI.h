#pragma once

#include <glm/glm.hpp>
#include "Stulu/Renderer/VertexArray.h"

namespace Stulu {
	class RenderAPI {
	public:
		enum class API {
			none = 0,
			OpenGL = 1, Vulkan = 2,
			Direct3D11 = 3, Direct3D12 = 4,
			Metal = 5, GLES = 6
		};

		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;

		virtual void drawIndex(const std::shared_ptr<VertexArray>& vertexArray) = 0;

		inline static API getAPI() { return s_api; };
	private:
		static API s_api;
	};
}