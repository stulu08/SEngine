#include "st_pch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Stulu/Core/Application.h"

namespace Stulu {

	OpenGLContext::OpenGLContext() 
	{ }

	void OpenGLContext::init(Window* window) {
		ST_PROFILING_FUNCTION();
		{
			GLFWwindow* windowHandle = static_cast<GLFWwindow*>(window->getNativeWindow());
			CORE_ASSERT(windowHandle, "Window handle is null");
			m_windowHandle = windowHandle;
		}
		glfwMakeContextCurrent(m_windowHandle);
		int32_t status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CORE_ASSERT(status, "Failed glad initialization");
		CORE_INFO("Renderer Data:\nVendor: {0}\nRenderer: {1}\nVersion: {2}", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION))
	}

	void OpenGLContext::swapBuffers() {
		
		glfwSwapBuffers(m_windowHandle);
	}
	void OpenGLContext::setVSync(bool enabled) {
		ST_PROFILING_FUNCTION();
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}
	API_Infos OpenGLContext::getApiInfos() {
		return API_Infos{ "OpenGL",(char*)glGetString(GL_VENDOR),(char*)glGetString(GL_VERSION),(char*)glGetString(GL_RENDERER) };
	}
}
