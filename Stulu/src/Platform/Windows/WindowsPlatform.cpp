#include "st_pch.h"
#ifdef ST_PLATFORM_WINDOWS
#include "Stulu/Core/Platform.h"
#include <GLFW/glfw3.h>

namespace Stulu {
	float Platform::getTime() {
		return (float)glfwGetTime();
	}
}
#endif // PLAFORM_WINDOWS