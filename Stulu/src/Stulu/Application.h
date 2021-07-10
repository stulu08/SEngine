#pragma once

#include "Core.h"
#include <Stulu/Window.h>

namespace Stulu {
	class STULU_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
	private:
		std::unique_ptr<Window> m_window;
		bool m_runnig = true;
	};

	//defined in Client
	Application* CreateApplication();
}

