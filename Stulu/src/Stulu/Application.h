#pragma once

#include "Core.h"
#include "Window.h"
#include "Stulu/LayerStack.h"
#include "Stulu/ImGui/ImGuiLayer.h"
#include "Stulu/Events/ApplicationEvent.h"

namespace Stulu {
	class STULU_API Application
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
	private:
		bool onWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imguiLayer;
		bool m_runnig = true;
		LayerStack m_layerStack;
	private:
		static Application* s_instance;
	};

	//defined in Client
	Application* CreateApplication();
}

