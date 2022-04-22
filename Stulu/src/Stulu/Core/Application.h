#pragma once

#include "Stulu/Core/Window.h"
#include "Stulu/Core/LayerStack.h"
#include "Stulu/ImGui/ImGuiLayer.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Core/Version.h"
#include "Stulu/Core/Time.h"

namespace Stulu {
	struct ApplicationInfo {
		const char* ApplicationName;
		const Version ApplicationVersion;
		WindowProps windowProps;

		ApplicationInfo(const char* applicationName = "Stulu Engine", const Version applicationVersion = Version())
			: ApplicationName(applicationName), ApplicationVersion(applicationVersion) {
			windowProps = WindowProps(applicationName);
		}
		ApplicationInfo(const char* applicationName, const Version applicationVersion, WindowProps windowProps)
			: ApplicationName(applicationName), ApplicationVersion(applicationVersion), windowProps(windowProps) {}
	};
	class STULU_API AssemblyManager;
	class STULU_API Application {
	public:
		Application(ApplicationInfo appInfo, bool hideWindow = false, bool enableImgui = true, const std::string& defaultAssetsPath = "assets");
		virtual ~Application();

		void run();
		void onEvent(Event& e);
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);
		void popLayer(Layer* layer);
		void popOverlay(Layer* layer);
		inline ImGuiLayer* getImGuiLayer() const { return m_imguiLayer; }
		inline Window& getWindow() const { return *m_window; }
		inline const ApplicationInfo& getApplicationInfo() const { return m_appInfo; }

		inline const Ref<AssemblyManager>& getAssemblyManager() const { return m_assembly; }

		inline static Application& get(){ return *s_instance; }
		inline static uint32_t getWidth() { return s_instance->m_window->getWidth(); }
		inline static uint32_t getHeight(){ return s_instance->m_window->getWidth(); }
		inline static const std::string& getEngineAssetDir() { return s_instance->m_assetPath; }

		static inline std::vector<std::string>& getStartArgs() { return s_startArgs; }
		static inline const std::string getStartDirectory() { return s_startArgs[0].substr((size_t)0, s_startArgs[0].find_last_of("/\\")); }

		static void exit(int32_t code = 0);
	protected:
		Ref<AssemblyManager> m_assembly = nullptr;
	private:
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);

		std::string m_assetPath;
		Scope<Window> m_window;
		ImGuiLayer* m_imguiLayer;
		LayerStack m_layerStack;
		ApplicationInfo m_appInfo;
		bool m_runnig = false;
		bool m_minimized = false;
		bool m_enableImgui = false;

		float m_lastFrameTime = 0.0f;


		static Application* s_instance;
		static inline std::vector<std::string> s_startArgs;
	};

	//defined in Client
	Application* CreateApplication();
}

