#include "st_pch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Stulu {


	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) 
		: m_windowHandle(windowHandle)
	{CORE_ASSERT(windowHandle,"Window hadle is null") }

	void OpenGLContext::init() {
		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CORE_ASSERT(status, "Failed glad initialization");

		CORE_INFO("OpenGL {0} Renderer: {1}",glGetString(GL_VENDOR), glGetString(GL_RENDERER));
		CORE_INFO("	version: {0}", glGetString(GL_VERSION));
		CORE_INFO("	GLSL shading version: {0}", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	void OpenGLContext::swapBuffers() {

		glfwSwapBuffers(m_windowHandle);
	}
}
