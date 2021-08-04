#include "st_pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Stulu{
	Shader* Shader::create(const std::string& path, bool multiFile) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return new OpenGLShader(path, multiFile);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Shader creation")
			return nullptr;
	}
	Shader* Shader::create(const std::string& vertex, const std::string& fragment) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return new OpenGLShader(vertex, fragment);
		case RenderAPI::API::none:
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