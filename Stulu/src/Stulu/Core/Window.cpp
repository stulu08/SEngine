#include "st_pch.h"
#include "Window.h"

#include "Platform/Windows/GLwindowsWindow.h"
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	Scope<Window> Window::create(const WindowProps& props) {
		switch (Renderer::getRendererAPI())
		{
			case RenderAPI::API::OpenGL:
				return createScope<GLwindowsWindow>(props);
			case RenderAPI::API::none:
				CORE_ASSERT(false, "No renderAPI specified");
				return nullptr;
			default:
				CORE_ASSERT(false, "RenderAPI not suported");
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Window creation");
		return nullptr;
	}
}