#pragma once

#ifdef ST_PLATFORM_WINDOWS
#include <Windows.h>
	extern Stulu::Application* Stulu::CreateApplication();
	int main(int argc, char** argv) {
#if ST_SHOWCONSOLE
		ShowWindow(GetConsoleWindow(), SW_SHOW);
#else
		ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif // ST_SHOWCONSOLE

		ST_PROFILING_BEGIN("Startup", "Profiling-Startup.json");
		Stulu::Log::init();
		for (int i = 0; i < argc; i++)
			Stulu::Application::getStartArgs().push_back(argv[i]);
		auto application = Stulu::CreateApplication();
		ST_PROFILING_END();

		ST_PROFILING_BEGIN("Runtime", "Profiling-Runtime.json");
		application->run();
		ST_PROFILING_END();

		ST_PROFILING_BEGIN("Shutdown", "Profiling-Shutdown.json");
		delete application;
		CORE_INFO("Ended");
		ST_PROFILING_END();

		return 0;
	}

#endif
