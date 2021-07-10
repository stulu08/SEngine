#include "Application.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Log.h"

namespace Stulu {
	Application::Application() {

	}
	Application::~Application() {
		
	}
	void Application::run() {

		WindowResizeEvent e(1920, 1080);
		ST_TRACE(e);

		CORE_INFO("Starting run loop");
		while (true);
	}
}