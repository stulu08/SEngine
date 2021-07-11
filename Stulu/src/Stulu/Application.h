#pragma once

#include "Core.h"
#include "Events/Event.h"
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
	private:
		bool onWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_window;
		bool m_runnig = true;
	};

	//defined in Client
	Application* CreateApplication();
}

