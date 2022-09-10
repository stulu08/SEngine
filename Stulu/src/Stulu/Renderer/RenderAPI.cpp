#include "st_pch.h"
#include "RenderAPI.h"

namespace Stulu{
#ifdef OPENGL
	RenderAPI::API RenderAPI::s_api = RenderAPI::API::OpenGL;
#else 
	#ifdef VULKAN
		RenderAPI::API RenderAPI::s_api = RenderAPI::API::Vulkan;
	#endif
#endif
}