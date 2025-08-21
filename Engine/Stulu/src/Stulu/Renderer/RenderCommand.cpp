#include "st_pch.h"
#include "RenderCommand.h"
#include "Renderer.h"

#include "Platform/Vulkan/VulkanRenderAPI.h"
#include "Platform/OpenGL/OpenGLRenderAPI.h"

namespace Stulu {
	Scope<RenderAPI> RenderCommand::s_renderAPI = nullptr;


	void RenderCommand::init() {
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			s_renderAPI = createScope<OpenGLRenderAPI>();
			break;
		case Renderer::API::Vulkan:
			s_renderAPI = createScope<VulkanRenderAPI>();
			break;
		case Renderer::API::none:
			CORE_ASSERT(false, "No Render API specified")
			break;
		default:
			CORE_ASSERT(false, "Render API not suported")
			break;
		}
		s_renderAPI->init();
	}

}