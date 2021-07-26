#pragma once

namespace Stulu{
	enum class RendererAPI { none = 0, 
		OpenGL = 1, Vulkan = 2, 
		Direct3D11 = 3, Direct3D12 = 4, 
		Metal = 5, GLES = 6
	};

	class Renderer {

	public:
		inline static RendererAPI getRendererAPI() { return s_rendererAPI; }
	private:
		static RendererAPI s_rendererAPI;
	};

}