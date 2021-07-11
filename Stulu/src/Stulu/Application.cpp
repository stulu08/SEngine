#include "st_pch.h"
#include "Application.h"

#include "Log.h"

#include <GLFW/glfw3.h>

namespace Stulu {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application() {
		m_window = std::unique_ptr<Window>(Window::create());
		m_window->setEventCallback(BIND_EVENT_FN(onEvent));
	}
	Application::~Application() {
		
	}
	void Application::onEvent(Event& e) {
		EventDispatcher dispacther(e);
		dispacther.dispacth<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));
		//CORE_TRACE("{0}",e);
	}
	void Application::run() {
		m_window->setVSysnc(true);
		while (m_runnig) {
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_window->onUpdate();
		}
	}
	bool Application::onWindowClose(WindowCloseEvent& e) {
		m_runnig = false;
		return m_runnig;
	}
}