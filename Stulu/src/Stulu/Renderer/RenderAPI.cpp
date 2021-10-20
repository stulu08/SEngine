#include "st_pch.h"
#include "RenderAPI.h"

namespace Stulu{
#if OPENGL
	RenderAPI::API RenderAPI::s_api = RenderAPI::API::OpenGL;
#elif VULKAN
	RenderAPI::API RenderAPI::s_api = RenderAPI::API::Vulkan;
#endif
}