#pragma once


namespace Stulu {
	extern Application* Stulu::CreateApplication();

	int main_entry(int argc, char** argv) {

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
}

#ifdef ST_PLATFORM_WINDOWS
#include <Windows.h>
#ifdef ST_DIST
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
	return Stulu::main_entry(__argc, __argv);
}
#else
int main(int argc, char** argv) {
	return Stulu::main_entry(argc, argv);
}
#endif //ST_DIST
#endif //ST_PLATFORM_WINDOWS
