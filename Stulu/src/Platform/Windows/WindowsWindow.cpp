#include "st_pch.h"
#include "WindowsWindow.h"

namespace Stulu {
	static bool s_glfwInitilized = false;

	Window* Window::create(const WindowProps& props) {
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) {
		init(props);
	}
	WindowsWindow::~WindowsWindow() { shutDown(); }

	void WindowsWindow::init(const WindowProps& props) {
		m_data.title = props.title;
		m_data.width = props.width;
		m_data.height = props.height;

		CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (!s_glfwInitilized) {

			int succes = glfwInit();
			CORE_ASSERT(succes, "Could not initialize GLFW!");
			s_glfwInitilized = true;
		}

		m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window, &m_data);
		setVSysnc(true);
	}
	void WindowsWindow::shutDown() {
		glfwDestroyWindow(m_window);
	}
	void WindowsWindow::onUpdate() {
		glfwPollEvents();
		glfwSwapBuffers(m_window);
	}

	void WindowsWindow::setVSysnc(bool enabled) {
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_data.VSync = enabled;
	}

	bool WindowsWindow::isVSysnc() const { return m_data.VSync; }
}