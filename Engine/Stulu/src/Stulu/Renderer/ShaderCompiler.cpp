#include "st_pch.h"
#include "ShaderCompiler.h"

#include "Platform/Vulkan/SpirvShaderCompiler.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer.h"

namespace Stulu {
    Ref<ShaderCompiler> Stulu::ShaderCompiler::Create() {
		if (ShaderSystem::CheckSpirv()) {
			return std::make_shared<SpirVShaderCompiler>();
		}

		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return std::make_shared<OpenGLShaderCompiler>();
		//case Renderer::API::Vulkan:
			//return std::make_shared<SpirVShaderCompiler>();

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

