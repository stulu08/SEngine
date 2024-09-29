#include "App.h"
#include <Stulu/Core/EntryPoint.h>

#include "Stulu/Scene/YAML.h"

namespace Stulu {
	Application* Stulu::CreateApplication(int argc, char** argv) {
		Log::AddFileSink("Logs/" + Log::generateTimeString() + ".log", Log::Level::warn);
		
		ApplicationInfo info;
		info.Name = "Your Project";
		info.Publisher = "Publisher";
		info.Version = Version(1, 0, 0);
		info.WindowProps.width = 1280;
		info.WindowProps.height = 720;
		info.DataPath = "Data";
		info.AppPath = info.DataPath + "/Assets";
		info.AppManagedAssembly = info.DataPath + "/Stulu/Managed/ManagedAssembly.dll";
		info.HideWindowOnSart = false;
		info.EnableImgui = false;
		info.LoadDefaultAssets = true;
		return new RuntimeApp(info);
	}

	RuntimeApp::RuntimeApp(const ApplicationInfo& info) 
		: Application(info) {

		AssetsManager::loadAllFiles(info.AppPath);
		pushLayer(new RuntimeLayer());

		getWindow().setVSync(false);
		getWindow().show();
	}
	RuntimeApp::~RuntimeApp() {
		
	}
}