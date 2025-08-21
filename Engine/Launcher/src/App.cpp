#include "App.h"
#include <Stulu/Core/EntryPoint.h>

#include "Stulu/Serialization/YAML.h"

Stulu::Application* Stulu::CreateApplication(int argc, char** argv) {
	ApplicationInfo info;
	info.Name = "Stulu Launcher";
	info.Publisher = "Stulu";
	info.Version = ST_ENGINE_VERSION;
	info.WindowProps.width = 1280;
	info.WindowProps.height = 720;
	info.HideWindowOnSart = true;
	info.EnableImgui = true;
	info.LoadDefaultAssets = false;
	return new Stulu::Launcher::LauncherApp(info);
}

namespace Stulu::Launcher {
	LauncherApp* LauncherApp::s_instance = nullptr;

	LauncherApp::LauncherApp(const ApplicationInfo& info)
		: Application(info) {
		getWindow().setVSync(true);
		getWindow().setWindowTitle(std::string("Stulu Launcher V") + ST_ENGINE_VERSION_STR);
		s_instance = this;
		m_layer = new LauncherLayer();
		pushLayer(m_layer);
		getWindow().show();
	}
	LauncherApp::~LauncherApp() {}
}