#include "st_pch.h"
#include "Application.h"

#include "Stulu/Core/Platform.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Renderer/Renderer2D.h"
#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Core/Input.h"
#include "Stulu/Scene/Resources.h"
#include "Stulu/ScriptCore/AssemblyManager.h"

namespace Stulu {
	// we want the best gpu cause shaders wont compile with intel uhd graphics
	// enable optimus!
	extern "C" {
		_declspec(dllexport) DWORD NvOptimusEnablement = 1;
		_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
	}
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::s_instance = nullptr;

	Application::Application(ApplicationInfo appInfo, bool hideWindow, bool enableImgui, const std::string& defaultAssetsPath)
		:m_appInfo(appInfo), m_enableImgui(enableImgui), m_assetPath(defaultAssetsPath) {
		ST_PROFILING_FUNCTION();
		s_instance = this;
		m_window = Window::create(m_appInfo.windowProps);
		m_window->setEventCallback(BIND_EVENT_FN(onEvent));
		if (hideWindow)
			m_window->hide();
		CORE_INFO("Loading all Engine assets from: {0}/{1}", getStartDirectory(), m_assetPath);
		AssetsManager::loadAllFiles(m_assetPath);

		Renderer::init();
		if (m_enableImgui) {
			m_imguiLayer = new ImGuiLayer();
			pushOverlay(m_imguiLayer);
		}
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

		Input::onEvent(e);

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			if (e.handled)
				break;
			(*--it)->onEvent(e);
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
			if (!m_minimized) {
				for (Layer* layer : m_layerStack) {
					ST_PROFILING_SCOPE("onUpdate - layerstack");
					layer->onUpdate(delta);
				}
				if (m_enableImgui) {
					m_imguiLayer->Begin();
					for (Layer* layer : m_layerStack) {
						ST_PROFILING_SCOPE("onImguiRender - layerstack");
						layer->onImguiRender(delta);
					}
					for (Layer* layer : m_layerStack) {
						ST_PROFILING_SCOPE("onRenderGizmo - layerstack");
						layer->onRenderGizmo();
					}
					m_imguiLayer->End();
				}
			}
			Input::update();
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
		exit(0);
		return m_runnig;
	}
	bool Application::onWindowResize(WindowResizeEvent& e)
	{
		m_minimized = e.getWidth() == 0 || e.getHeight() == 0;
		m_minimized ? 0 : Renderer::onWindowResize(e);
		return m_minimized;
	}
}