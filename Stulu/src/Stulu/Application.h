#pragma once

#include "Core.h"
#include "Log.h"

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

