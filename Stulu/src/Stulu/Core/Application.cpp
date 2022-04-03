#include "st_pch.h"
#include "Application.h"

#include "Stulu/Core/Platform.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Renderer/Renderer2D.h"
#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Core/Input.h"
#include "Stulu/Scene/Resources.h"

namespace Stulu {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::s_instance = nullptr;

	Application::Application(ApplicationInfo appInfo, bool hideWindow)
		:m_appInfo(appInfo)	{
		ST_PROFILING_FUNCTION();
		s_instance = this;
		m_window = Window::create(m_appInfo.windowProps);
		m_window->setEventCallback(BIND_EVENT_FN(onEvent));
		if (hideWindow)
			m_window->hide();
		CORE_INFO("Loading all Engine assets from: {0}/{1}", getStartDirectory(),"assets");
		AssetsManager::loadAllFiles("assets");
#if OPENGL
		Renderer::init();
		m_imguiLayer = new ImGuiLayer();
		pushOverlay(m_imguiLayer);
#endif
		m_scriptCore = createRef<ScriptCore>(".",".");
		float e = 1.0f;
		void* p[1];
		p[0] = &e;
		m_scriptCore->getEngineObject()->callConstructor("(single)", p);
		
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

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*--it)->onEvent(e);
			if (e.handled)
				break;
		}

	}
	void Application::run() {
		ST_PROFILING_FUNCTION();
		m_lastFrameTime = Platform::getTime();
		m_runnig = true;
		while (m_runnig) {
			ST_PROFILING_SCOPE("Run Loop");
			float time = Platform::getTime();
			Timestep delta = time - m_lastFrameTime;
			Time::applicationRuntime = time;
			m_lastFrameTime = time;
			Time::frameTime = delta;
			Time::deltaTime = delta * Time::Scale;
			Input::update();
			m_scriptCore->getEngineObject()->call("onUpdate()");
			if (!m_minimized) {
				for (Layer* layer : m_layerStack) {
					ST_PROFILING_SCOPE("onUpdate - layerstack");
					layer->onUpdate(delta);
				}
#if OPENGL
				m_imguiLayer->Begin();
				for (Layer* layer : m_layerStack) {
					ST_PROFILING_SCOPE("onImguiRender - layerstack");
					layer->onImguiRender(delta);
					layer->onRenderGizmo();
				}
				m_imguiLayer->End();
#endif
			}
			m_window->onUpdate();
		}
	}
	void Application::exit(int32_t code) {
		ST_PROFILING_FUNCTION();
		if (s_instance == nullptr) {
			CORE_CRITICAL("Application can't shut down! It hasn't been created yet");
			return;
		}
		if (s_instance->m_runnig == false) {
			CORE_WARN("Shutting down while loading!");
			CORE_INFO("Force Exit: {0}", code);
			::exit(code);
			return;
		}
		s_instance->m_runnig = false;
		CORE_INFO("Force Exit: {0}", code);
		return;
	}
	bool Application::onWindowClose(WindowCloseEvent& e) {
		ST_PROFILING_FUNCTION();
		m_runnig = false;
		return m_runnig;
	}
	bool Application::onWindowResize(WindowResizeEvent& e)
	{
		m_minimized = e.getWidth() == 0 || e.getHeight() == 0;
		m_minimized ? 0 : Renderer::onWindowResize(e);
		return m_minimized;
	}
}