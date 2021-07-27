#include "st_pch.h"
#include "PlatformWindows.h"

#include <GLFW/glfw3.h>

namespace Stulu {
    Platform* Platform::s_instance = new PlatformWindows();
    float PlatformWindows::impl_getTime()
    {
        return (float)glfwGetTime();
    }
}
