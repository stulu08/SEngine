#include "st_pch.h"
#include "Application.h"

#include "Stulu/Core/Platform.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/Renderer2D.h"
#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Core/Input.h"
#include "Stulu/Core/Resources.h"
#include "Stulu/Scripting/Managed/AssemblyManager.h"
#include "Stulu/ImGui/Gizmo.h"

namespace Stulu {
#ifdef OPENGL
	// we want the best gpu cause shaders wont compile with intel uhd graphics on my laptop and windows does not want to use my nvidia card automaticly
	// enable optimus!
	extern "C" {
		_declspec(dllexport) DWORD NvOptimusEnablement = 1;
		_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
	}
#endif

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::s_instance = nullptr;

	Application::Application(const ApplicationInfo& appInfo)
		:m_appInfo(appInfo) {
		ST_PROFILING_FUNCTION();
		s_instance = this;

		Resources::EngineDataDir = appInfo.DataPath;
		Resources::AppDataDir = appInfo.AppPath;
		Resources::AppAssetDir = appInfo.AppAssetPath;

		Renderer::s_data.api = appInfo.api;
		
		m_window = Window::create(m_appInfo.WindowProps);
		m_window->setEventCallback(BIND_EVENT_FN(onEvent));

		RenderCommand::init();

		if (appInfo.LoadDefaultAssets) {
			Renderer::init();
			LoadingScreen(0.0f);
		}

		if (appInfo.HideWindowOnSart) {
			m_window->hide();
		}

		if (appInfo.LoadDefaultAssets) {
			CORE_INFO("Loading all Engine assets: {0}", appInfo.DataPath);
			Resources::load();
		}

		if(!appInfo.AppAssembly.empty())
			m_assembly = createRef<AssemblyManager>(appInfo.AppAssembly, Resources::EngineDataDir + "/Stulu/Managed/Stulu.ScriptCore.dll");

		if (m_appInfo.EnableImgui) {
			m_imguiLayer = new ImGuiLayer();
			pushOverlay(m_imguiLayer);
		}

		if (appInfo.LoadDefaultAssets) {
			Gizmo::init();
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

	const Ref<ScriptAssembly>& Application::getScriptCoreAssembly() const {
		return m_assembly->getScriptCoreAssembly();
	}

	void Application::onEvent(Event& e) {
		ST_PROFILING_FUNCTION();
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
		m_lastFrameTime = Platform::getTime();
		m_runnig = true;
		while (m_runnig) {
			ST_PROFILING_SCOPE("Run Loop");
			// update input
			m_window->onUpdate();
			Input::update();

			// begin render command buffer
			m_window->getContext()->beginBuffer();

			// update timing
			float time = Platform::getTime();
			Timestep delta = time - m_lastFrameTime;
			Time::applicationRuntime = time;
			m_lastFrameTime = time;
			Time::frameTime = delta;
			Time::deltaTime = delta * Time::Scale;
			
			if (!m_minimized) {
				ST_PROFILING_RENDERDATA_BEGIN();
				// update every layer
				for (Layer* layer : m_layerStack) {
					layer->onUpdate(delta);
				}

				if (m_appInfo.EnableImgui) {
					// update imgui
					ST_PROFILING_SCOPE("ImGui - Update");
					m_imguiLayer->Begin();
					for (Layer* layer : m_layerStack) {
						layer->onImguiRender(delta);
					}
					for (Layer* layer : m_layerStack) {
						layer->onRenderGizmo();
					}
					m_imguiLayer->End();
				}
				ST_PROFILING_RENDERDATA_END();
			}

			// update events, end render buffer and swap buffers
			m_window->getContext()->swapBuffers();
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
	bool Application::onWindowResize(WindowResizeEvent& e) {
		ST_PROFILING_FUNCTION();
		m_minimized = e.getWidth() == 0 || e.getHeight() == 0;
		m_minimized ? 0 : Renderer::onWindowResize(e);
		return m_minimized;
	}
	std::string Application::getWorkingDirectory() const {
		return CleanPath(Platform::getCurrentWorkingDirectory());
	}
	void Application::LoadingScreen(float progress) {
		const auto& app = s_instance;

		if (app->getApplicationInfo().HideWindowOnSart)
			return;

		app->getWindow().getContext()->beginBuffer();

		//Load Texture for loading
		Renderer::onWindowResize(WindowResizeEvent(app->getWidth(), app->getHeight()));

		const float zoom = 0.75f;
		const float aspectRatio = (float)app->getWidth() / (float)app->getHeight();
		const glm::mat4 proj = glm::ortho(zoom * -aspectRatio, zoom * aspectRatio, zoom * -1.0f, zoom * 1.0f, .001f, 100.0f);
		const glm::mat4 view = glm::inverse(Math::createMat4(glm::vec3(.0f, .0f, .1f), glm::quat(glm::vec3(.0f)), glm::vec3(1.0f)));

		Ref<Texture> texture = Resources::getLoadingTexture();

		RenderCommand::setClearColor({ 0,0,0,1 });
		RenderCommand::clear();

		Renderer::uploadCameraBufferData(proj, view, glm::vec3(.0f, .0f, .1f), glm::vec3(.0f));
		Renderer2D::begin();
		Renderer2D::drawTexturedQuad(Math::createMat4(glm::vec3(.0f, .1f, .0f), glm::vec3(.75f)), texture);
		Renderer2D::drawSlider(glm::vec3(.0f, -.3f, .0f), glm::vec3(1.f, .1f, 1.0f), progress);
		Renderer2D::flush();


		app->getWindow().onUpdate();
		app->getWindow().getContext()->swapBuffers();
	}
}