#pragma once

#include "Stulu/Core/Window.h"
#include "Stulu/Core/LayerStack.h"
#include "Stulu/ImGui/ImGuiLayer.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Core/Version.h"
#include "Stulu/Core/Time.h"
#include "Stulu/Core/Platform.h"
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	struct ApplicationInfo {
		std::string Name;
		std::string Publisher;
		Version Version;
		WindowProps WindowProps;

		std::string DataPath = "Data/";
		std::string AppPath;
		std::string AppCachePath;
		std::string AppAssetPath;
		std::string AppAssembly;

		Renderer::API api = Renderer::API::OpenGL;

		bool HideWindowOnSart;
		bool EnableImgui;
		// this includes default assets, gizmo and renderer
		bool LoadDefaultAssets;
	};
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

		inline const Ref<AssemblyManager>& getAssemblyManager() const { return m_assembly; }
		inline Ref<AssemblyManager>& getAssemblyManager() { return m_assembly; }
		const Ref<ScriptAssembly>& getScriptCoreAssembly() const;
		inline ImGuiLayer* getImGuiLayer() const { return m_imguiLayer; }
		inline Window& getWindow() const { return *m_window; }
		
		static inline Application& get() { return *s_instance; }
		inline const ApplicationInfo& getApplicationInfo() const { return m_appInfo; }
		
		inline uint32_t getWidth() const { return s_instance->m_window->getWidth(); }
		inline uint32_t getHeight() const { return s_instance->m_window->getHeight(); }
		inline float getAspectRatio() const { return (float)getWidth() / (float)getHeight(); }
		
		std::string getWorkingDirectory() const;

		static void LoadingScreen(float progress);

		void exit(int32_t code = 0);
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
	};

	//defined in Client
	Application* CreateApplication(int argc, char** argv);
}

