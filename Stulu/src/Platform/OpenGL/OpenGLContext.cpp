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
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CORE_ASSERT(status, "Failed glad initialization");

		Application::get().graphicDriverConstants = GraphicDriverConstants(
			(const char*)glGetString(GL_VENDOR),
			(const char*)glGetString(GL_RENDERER), 
			(const char*)glGetString(GL_VERSION), 
			(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)
		);

	}

	void OpenGLContext::swapBuffers() {

		glfwSwapBuffers(m_windowHandle);
	}
}
