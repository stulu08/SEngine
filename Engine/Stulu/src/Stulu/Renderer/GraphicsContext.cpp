#include "st_pch.h"
#include "GraphicsContext.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/WindowsVulkanContext.h"

namespace Stulu {
	Scope<GraphicsContext> GraphicsContext::create() {
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return createScope<OpenGLContext>();
		case Renderer::API::Vulkan:
			return createScope<WindowsVulkanContext>();
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