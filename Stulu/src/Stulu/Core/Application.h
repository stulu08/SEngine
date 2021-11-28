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


	class STULU_API Application {
	public:
		Application(ApplicationInfo appInfo);
		virtual ~Application();

		void run();
		void onEvent(Event& e);
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);
		void popLayer(Layer* layer);
		void popOverlay(Layer* layer);
		inline ImGuiLayer* getImGuiLayer(){ return m_imguiLayer; }
		inline Window& getWindow(){ return *m_window; }
		inline const ApplicationInfo& getApplicationInfo() { return m_appInfo; }

		inline static Application& get(){ return *s_instance; }

		static void exit(int code = 0);
	private:
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);

		Scope<Window> m_window;
		ImGuiLayer* m_imguiLayer;
		LayerStack m_layerStack;
		ApplicationInfo m_appInfo;
		bool m_runnig = true;
		bool m_minimized = false;
		float m_lastFrameTime;

		static Application* s_instance;
	};

	//defined in Client
	Application* CreateApplication();
}

