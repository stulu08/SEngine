#pragma once

#include "Stulu/Renderer/RenderCommand.h"

namespace Stulu{
	class Renderer {

	public:
		static void beginScene();
		static void endScene();

		static void submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RenderAPI::API getRendererAPI() { return RenderAPI::getAPI(); }
	};

}