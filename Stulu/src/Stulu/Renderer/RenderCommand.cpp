#include "st_pch.h"
#include "RenderCommand.h"

#include "Platform/Vulkan/VulkanRenderAPI.h"
#include "Platform/OpenGL/OpenGLRenderAPI.h"

namespace Stulu {
#ifdef OPENGL
	Scope<RenderAPI> RenderCommand::s_renderAPI = createScope<OpenGLRenderAPI>();
#else 
	#ifdef VULKAN
		Scope<RenderAPI> RenderCommand::s_renderAPI = createScope<VulkanRenderAPI>();
	#endif
#endif

}