#include "st_pch.h"
#include "VertexArray.h"

#include "Stulu/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Stulu {
	Ref<VertexArray> VertexArray::create() {
		switch (Renderer::getRendererAPI())
		{
			case Renderer::API::OpenGL:
				return std::make_shared<OpenGLVertexArray>();
			case Renderer::API::none:
				CORE_ASSERT(false, "No renderAPI specified");
				return nullptr;
			default:
				CORE_ASSERT(false, "RenderAPI not suported");
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in VertexArray creation");
		return nullptr;
	}

}