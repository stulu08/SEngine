#include "st_pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Stulu{
	Shader* Shader::create(const std::string& vertex, const std::string& fragment) {
		switch (Renderer::getRendererAPI())
		{
		case RendererAPI::OpenGL:
			return new OpenGLShader(vertex, fragment);
		case RendererAPI::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Shader creation")
			return nullptr;
	}
}