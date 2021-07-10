#pragma once

#include "Core.h"

namespace Stulu {
	class STULU_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
	};

	//defined in Client
	Application* CreateApplication();
}

