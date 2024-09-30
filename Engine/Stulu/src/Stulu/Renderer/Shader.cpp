#include "st_pch.h"
#include "Shader.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include <Platform/Vulkan/VulkanShader.h>

namespace Stulu{
	Ref<Shader> Shader::create(const std::string& name, const ShaderCompileResult& sources) {
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, sources);
		case Renderer::API::Vulkan:
			return std::make_shared<VulkanShader>(name, sources);
		case Renderer::API::none:
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