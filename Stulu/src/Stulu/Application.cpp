#include "st_pch.h"
#include "Application.h"

#include "Log.h"

#include <glad/glad.h>

namespace Stulu {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::s_instance = nullptr;
	Application::Application() {
		s_instance = this;
		m_window = std::unique_ptr<Window>(Window::create());
		m_window->setEventCallback(BIND_EVENT_FN(onEvent));

		m_imguiLayer = new ImGuiLayer();
		pushOverlay(m_imguiLayer);
	}
	Application::~Application() {
		
	}
	void Application::pushLayer(Layer* layer) { m_layerStack.pushLayer(layer); }
	void Application::pushOverlay(Layer* layer) { m_layerStack.pushOverlay(layer); }

	void Application::onEvent(Event& e) {
		EventDispatcher dispacther(e);
		dispacther.dispacth<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*--it)->onEvent(e);
			if (e.handled)
				break;
		}

	}
	void Application::run() {
		while (m_runnig) {
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);


			for (Layer* layer : m_layerStack)
				layer->onUpdate();


			m_imguiLayer->Begin();
			for (Layer* layer : m_layerStack)
				layer->onImguiRender();
			m_imguiLayer->End();

			m_window->onUpdate();
		}
	}
	bool Application::onWindowClose(WindowCloseEvent& e) {
		m_runnig = false;
		return m_runnig;
	}
}