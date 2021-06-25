#pragma once

#ifdef ST_PLATFORM_WINDOWS

extern Stulu::Application* Stulu::CreateApplication();

int main(int argc, char** argv) {

	auto app = Stulu::CreateApplication();
	app->run();
	delete app;
}

#endif
