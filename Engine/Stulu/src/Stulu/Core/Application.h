#pragma once

#include "Stulu/Core/Window.h"
#include "Stulu/Core/LayerStack.h"
#include "Stulu/ImGui/ImGuiLayer.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Core/Version.h"
#include "Stulu/Core/Time.h"
#include "Stulu/Core/Platform.h"

namespace Stulu {
	struct ApplicationInfo {
		std::string ApplicationName;
		std::string Publisher;
		Version ApplicationVersion;
		std::string DefaultAssetsPath;
		bool HideWindowOnSart;
		bool EnableImgui;
		// this includes default assets, gizmo and renderer
		bool LoadDefaultAssets;
		WindowProps windowProps;

		ApplicationInfo(
			const std::string& applicationName = "Stulu Engine", 
			const std::string& publisher = "Stulu",
			const Version& applicationVersion = Version(1,0,0),
			const std::string& defaultAssetsPath = "assets", 
			bool hideWindowOnSart = false, 
			bool EnableImgui = true,
			bool LoadDefaultAssets = true,
			const WindowProps& windowProps = WindowProps())
			:	ApplicationName(applicationName), Publisher(publisher), ApplicationVersion(applicationVersion),
				DefaultAssetsPath(defaultAssetsPath), HideWindowOnSart(hideWindowOnSart),
				EnableImgui(EnableImgui), LoadDefaultAssets(LoadDefaultAssets), windowProps(windowProps) { }
	};
	class STULU_API AssemblyManager;
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

		inline const Ref<AssemblyManager>& getAssemblyManager() const { return m_assembly; }
		inline Ref<AssemblyManager>& getAssemblyManager() { return m_assembly; }
		inline ImGuiLayer* getImGuiLayer() const { return m_imguiLayer; }
		inline Window& getWindow() const { return *m_window; }
		
		static inline Application& get() { return *s_instance; }
		inline const ApplicationInfo& getApplicationInfo() const { return m_appInfo; }
		
		static inline uint32_t getWidth() { return s_instance->m_window->getWidth(); }
		static inline uint32_t getHeight() { return s_instance->m_window->getHeight(); }
		static inline float getAspectRatio() { return (float)getWidth() / (float)getHeight(); }
		
		static inline std::vector<std::string>& getStartArgs() { return s_startArgs; }
		static inline const std::string& getEngineAssetDir() { return s_instance->m_appInfo.DefaultAssetsPath; }
		static inline const std::string getStartDirectory() { return Platform::getCurrentWorkingDirectory(); }

		static void exit(int32_t code = 0);
	protected:
		Ref<AssemblyManager> m_assembly = nullptr;
	private:
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);

		Scope<Window> m_window;
		ImGuiLayer* m_imguiLayer;
		LayerStack m_layerStack;
		ApplicationInfo m_appInfo;
		bool m_runnig = false;
		bool m_minimized = false;

		float m_lastFrameTime = 0.0f;

		static Application* s_instance;
		static inline std::vector<std::string> s_startArgs;
	};

	//defined in Client
	Application* CreateApplication();
}

