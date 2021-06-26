#pragma once

#ifdef ST_PLATFORM_WINDOWS

extern Stulu::Application* Stulu::CreateApplication();

int main(int argc, char** argv) {
	Stulu::Log::init();
	CORE_INFO("Logging was initilized");
	CORE_INFO("Starting...");
	auto application = Stulu::CreateApplication();
	application->run();
	delete application;
	CORE_INFO("Ended");
	return 0;
}

#endif
