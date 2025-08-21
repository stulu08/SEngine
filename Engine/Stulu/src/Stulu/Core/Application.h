#pragma once
#include "Stulu/Core/ApplicationInfo.h"
#include "Stulu/Core/CpuDispatcher.h"
#include "Stulu/Core/Layer.h"
#include "Stulu/Core/Module.h"
#include "Stulu/Core/Platform.h"
#include "Stulu/Core/Time.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/ImGui/ImGuiLayer.h"
#include "Stulu/Renderer/Window.h"
#include "Stulu/Types/Version.h"

namespace Stulu {
	class STULU_API AssemblyManager;
	class STULU_API ScriptAssembly;
	class STULU_API Application {
	public:
		Application(const ApplicationInfo& appInfo);
		virtual ~Application();

		void run();

		void onEvent(Event& e);
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);
		void popLayer(Layer* layer);
		void popOverlay(Layer* layer);

		// loads and registers the module
		template<class T = Module>
		inline static void LoadModule();

		inline const Ref<AssemblyManager>& getAssemblyManager() const { return m_assembly; }
		inline Ref<AssemblyManager>& getAssemblyManager() { return m_assembly; }
		const Ref<ScriptAssembly>& getScriptCoreAssembly() const;
		inline ImGuiLayer* getImGuiLayer() const { return m_imguiLayer; }
		inline Window& getWindow() const { return *m_window; }
		inline CpuDispatcher& GetCpuDispatcher() const { return *m_cpuDispatcher; }

		static Application& get();
		inline const ApplicationInfo& getApplicationInfo() const { return m_appInfo; }
		
		inline uint32_t getWidth() const { return get().m_window->getWidth(); }
		inline uint32_t getHeight() const { return get().m_window->getHeight(); }
		inline float getAspectRatio() const { return (float)getWidth() / (float)getHeight(); }
		
		std::string getWorkingDirectory() const;

		static void LoadingScreen(float progress);

		void exit(int32_t code = 0);
	protected:
		Ref<AssemblyManager> m_assembly = nullptr;
	private:
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);

		Scope<CpuDispatcher> m_cpuDispatcher;
		Scope<Window> m_window;
		Scope<AssetsManager> m_assetsManager;
		ImGuiLayer* m_imguiLayer;
		// scope would be better, but not compatible with std::vector. Since this is a private member it litirly doesnt matter
		Stack<Ref<Layer>> m_layerStack;
		// Stores the modules to be loaded, empty after Application constructor
		Stack<Module*> m_moduleLoader;

		ApplicationInfo m_appInfo;
		bool m_runnig = false;
		bool m_minimized = false;

		float m_lastFrameTime = 0.0f;

		static Application* s_instance;
	};

	//defined in Client
	Application* CreateApplication(int argc, char** argv);

	inline void Application::pushLayer(Layer* layer) {
		m_layerStack.push(Ref<Layer>(layer));
		layer->onAttach();
	}
	inline void Application::pushOverlay(Layer* layer) {
		m_layerStack.pushOverlay(Ref<Layer>(layer));
		layer->onAttach();
	}
	inline void Application::popLayer(Layer* layer) {
		m_layerStack.pop(Ref<Layer>(layer));
		layer->onDetach();
	}
	inline void Application::popOverlay(Layer* layer) {
		m_layerStack.popOverlay(Ref<Layer>(layer));
		layer->onDetach();
	}
	inline Application& Application::get() {
		return *s_instance;
	}
	template<class T>
	inline static void Application::LoadModule() {
		T* module = new T();
		if (module->onLoad(s_instance->m_appInfo)) {
			if constexpr (ModuleInfo<T>::IsOverlay)
				s_instance->m_moduleLoader.push(module);
			else
				s_instance->m_moduleLoader.pushOverlay(module);
		}
		else {
			delete module;
		}
	}
}

