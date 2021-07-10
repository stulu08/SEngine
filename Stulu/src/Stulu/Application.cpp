#include "st_pch.h"
#include "Application.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Log.h"

#include <GLFW/glfw3.h>

namespace Stulu {
	Application::Application() {
		m_window = std::unique_ptr<Window>(Window::create());
	}
	Application::~Application() {
		
	}
	void Application::run() {


		while (m_runnig) {
			glClearColor(0, 1, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_window->onUpdate();
		}
	}
}