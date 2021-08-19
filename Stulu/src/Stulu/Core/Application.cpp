#include "st_pch.h"
#include "Application.h"

#include "Stulu/Core/Platform.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Renderer/Renderer2D.h"

namespace Stulu {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::s_instance = nullptr;

	Application::Application() {
		ST_PROFILING_FUNCTION();
		s_instance = this;
		m_window = Window::create();
		m_window->setEventCallback(BIND_EVENT_FN(onEvent));
		Renderer::init();
		m_imguiLayer = new ImGuiLayer();
		pushOverlay(m_imguiLayer);
	}
	Application::~Application() {
		ST_PROFILING_FUNCTION();
		Renderer2D::shutdown();
		m_window.reset();
	}
	void Application::pushLayer(Layer* layer) {
		m_layerStack.pushLayer(layer);
	}
	void Application::pushOverlay(Layer* layer) {
		m_layerStack.pushOverlay(layer);
	}
	void Application::popLayer(Layer* layer) {
		m_layerStack.popLayer(layer);
	}
	void Application::popOverlay(Layer* layer) {
		m_layerStack.popOverlay(layer);
	}

	void Application::onEvent(Event& e) {
		EventDispatcher dispacther(e);
		dispacther.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));
		dispacther.dispatch<WindowResizeEvent>(BIND_EVENT_FN(onWindowResize));

		_onEvent(e);
		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*--it)->onEvent(e);
			if (e.handled)
				break;
		}

	}
	void Application::run() {
		ST_PROFILING_FUNCTION();
		while (m_runnig) {
			ST_PROFILING_SCOPE("Run Loop");
			float time = Platform::getTime();
			Timestep deltaTimestep = time - m_lastFrameTime;
			m_lastFrameTime = time;
			if (!m_minimized) {
				_onUpdate(deltaTimestep);
				for (Layer* layer : m_layerStack) {
					ST_PROFILING_SCOPE("onUpdate - layerstack");
					layer->onUpdate(deltaTimestep);
				}
			}
			m_imguiLayer->Begin();
			for (Layer* layer : m_layerStack) {
				ST_PROFILING_SCOPE("onImguiRender - layerstack");
				layer->onImguiRender(deltaTimestep);
			}
			m_imguiLayer->End();
			m_window->onUpdate();
		}
	}
	bool Application::onWindowClose(WindowCloseEvent& e) {
		ST_PROFILING_FUNCTION();
		m_runnig = false;
		return m_runnig;
	}
	bool Application::onWindowResize(WindowResizeEvent& e)
	{
		m_minimized = e.getWidth() == 0 || e.getHeight() == 0;
		m_minimized ? 0 : Renderer::onWinndowResize(e);
		return false;
	}
}