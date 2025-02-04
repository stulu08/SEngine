#include "App.h"
#include <Stulu/Core/EntryPoint.h>

#include "Stulu/Scene/YAML.h"
#include "Stulu/Runtime/RuntimeLayer.h"

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
		info.AppPath = "";
		info.AppAssetPath = info.DataPath + "/Assets";
		info.AppCachePath = "Cache/";
		info.AppManagedAssembly = info.DataPath + "/Stulu/Managed/ManagedAssembly.dll";
		info.AppNativeAssembly = info.DataPath + "/Stulu/Nativ/NativAssembly.dll";
		info.HideWindowOnSart = false;
		info.EnableImgui = false;
		info.LoadDefaultAssets = true;
		return new RuntimeApp(info);
	}

	RuntimeApp::RuntimeApp(const ApplicationInfo& info) 
		: Application(info) {
		
		AssetsManager::setProgessCallback(Application::LoadingScreen);
		AssetsManager::loadAllFiles(info.AppAssetPath);
		AssetsManager::setProgessCallback(nullptr);

		pushLayer(new RuntimeLayer("Data/Assets/scenes/Physx Test.scene"));

		getWindow().setVSync(false);
		getWindow().show();
	}
	RuntimeApp::~RuntimeApp() {
		
	}
}