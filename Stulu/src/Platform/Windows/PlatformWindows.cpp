#include "st_pch.h"
#include "PlatformWindows.h"

#include <GLFW/glfw3.h>

namespace Stulu {
    Scope<Platform> Platform::s_instance = createScope<PlatformWindows>();
    float PlatformWindows::impl_getTime()
    {
        return (float)glfwGetTime();
    }
}
