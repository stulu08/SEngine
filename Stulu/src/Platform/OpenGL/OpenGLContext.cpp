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
	}

	void OpenGLContext::swapBuffers() {

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, 1280, 720, 0.0f, 0.0f, 1.0f);
		glBegin(GL_POLYGON);
		glVertex2f(50.0f, 50.0f);
		glVertex2f(100.0f, 50.0f);
		glVertex2f(100.0f, 100.0f);
		glVertex2f(50.0f, 100.0f);
		glEnd();

		glfwSwapBuffers(m_windowHandle);
	}
}
