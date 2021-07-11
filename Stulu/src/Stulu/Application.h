#pragma once

#include "Core.h"
#include "Stulu/LayerStack.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Window.h"

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
	private:
		bool onWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_window;
		bool m_runnig = true;
		LayerStack m_layerStack;
	};

	//defined in Client
	Application* CreateApplication();
}

