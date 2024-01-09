#include "App.h"
#include <Stulu/Core/EntryPoint.h>

#include "Stulu/Scene/YAML.h"

Stulu::Application* Stulu::CreateApplication() {
	return new Stulu::Launcher::LauncherApp();
}

namespace Stulu::Launcher {
	LauncherApp* LauncherApp::s_instance = nullptr;

	LauncherApp::LauncherApp()
		: Application(ApplicationInfo("Stulu Launcher", "Stulu", Version(1, 0, 0), ".", true, true, false)) {
		getWindow().setVSync(true);
		getWindow().setWindowTitle(std::string("Stulu Launcher V") + ST_ENGINE_VERSION_STR);
		s_instance = this;
		m_layer = new LauncherLayer();
		getWindow().show();
		pushLayer(m_layer);
	}
	LauncherApp::~LauncherApp() {}
}