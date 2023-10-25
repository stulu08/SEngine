#include "App.h"
#include <Stulu/Core/EntryPoint.h>

#include "Stulu/Scene/YAML.h"

Stulu::Application* Stulu::CreateApplication() {
	return new Stulu::Launcher::LauncherApp();
}

namespace Stulu::Launcher {
	LauncherApp* LauncherApp::s_instance = nullptr;

	LauncherApp::LauncherApp()
		: Application(ApplicationInfo("Stulu Launcher", Version(1,0,0), ".", true, true, false)) {
		Log::addFileSink("logs/" + Log::generateTimeString() + ".log", Log::Level::warn);
		getWindow().setVSync(true);
		s_instance = this;
		m_layer = new LauncherLayer();
		getWindow().show();
		pushLayer(m_layer);
	}
	LauncherApp::~LauncherApp() {}
}