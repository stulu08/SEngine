#pragma once

#include "Stulu/Core/Window.h"
#include "Stulu/Core/LayerStack.h"
#include "Stulu/ImGui/ImGuiLayer.h"
#include "Stulu/Events/ApplicationEvent.h"

namespace Stulu {
	struct GraphicDriverConstants {
		std::string vendor;
		std::string renderer;
		std::string version;
		std::string other;

		GraphicDriverConstants(const std::string& _vendor = "no data", const std::string& _renderer = "no data", const std::string& _version = "no data", const std::string& _other = "no data")
			: vendor(_vendor), renderer(_renderer), version(_version), other(_other) {}
	};
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
		void onEvent(Event& e);
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);
		inline Window& getWindow(){ return *m_window; }
		inline static Application& get(){ return *s_instance; }

		GraphicDriverConstants graphicDriverConstants;
	private:
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);

		Scope<Window> m_window;
		ImGuiLayer* m_imguiLayer;
		bool m_runnig = true;
		bool m_minimized = false;
		LayerStack m_layerStack;
		float m_lastFrameTime;

		static Application* s_instance;
	};

	//defined in Client
	Application* CreateApplication();
}

