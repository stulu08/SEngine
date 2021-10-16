#include "st_pch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Stulu/Core/Application.h"

namespace Stulu {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) 
		: m_windowHandle(windowHandle)
	{CORE_ASSERT(windowHandle,"Window handle is null") }

	void OpenGLContext::init() {
		ST_PROFILING_FUNCTION();
		glfwMakeContextCurrent(m_windowHandle);
		int32_t status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CORE_ASSERT(status, "Failed glad initialization");
		CORE_INFO("Renderer Data:\nVendor: {0}\nRenderer: {1}\nVersion: {2}", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION))
	}

	void OpenGLContext::swapBuffers() {
		
		glfwSwapBuffers(m_windowHandle);
	}
}
