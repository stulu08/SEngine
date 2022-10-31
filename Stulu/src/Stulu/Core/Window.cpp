#include "st_pch.h"
#include "Window.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	Scope<Window> Window::create(void* handle) {
#ifdef ST_PLATFORM_WINDOWS
		return createScope<WindowsWindow>(handle);
#else
		CORE_ASSERT(false, "Platform not supported for window creation");
#endif // ST_PLATFORM_WINDOWS

		CORE_ASSERT(false, "Unknown error in Window creation");
		return nullptr;
	}
	Scope<Window> Window::create(WindowProps& props) {
#ifdef ST_PLATFORM_WINDOWS
		return createScope<WindowsWindow>(props);
#else
		CORE_ASSERT(false,"Platform not supported for window creation");
#endif // ST_PLATFORM_WINDOWS

		CORE_ASSERT(false, "Unknown error in Window creation");
		return nullptr;
	}
}