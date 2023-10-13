#include "App.h"
#include <Stulu/Core/EntryPoint.h>

#include "Stulu/Scene/YAML.h"

Stulu::Application* Stulu::CreateApplication() {
	return new Stulu::Launcher::LauncherApp();
}

namespace Stulu::Launcher {
	std::string readFile(const std::string& path) {
		std::ifstream inStream(path, std::ios::in | std::ios::binary);
		if (inStream) {
			std::string result;
			inStream.seekg(0, std::ios::end);
			result.resize(inStream.tellg());
			inStream.seekg(0, std::ios::beg);
			inStream.read(&result[0], result.size());
			inStream.close();
			return result;
		}
		CORE_ERROR("Could not open File: {0}", path);
		return "";
	}
	LauncherApp* LauncherApp::s_instance = nullptr;

	LauncherApp* runtimeLayer;

	LauncherApp::LauncherApp()
		: Application(ApplicationInfo("Stulu Launcher", Version(1,0,0), ".", true, true, false)) {
		Log::addFileSink("logs/" + Log::generateTimeString() + ".log", Log::Level::warn);
		getWindow().setVSync(true);
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		Resources::GameAssetDirectory = "/";
		s_instance = this;
		m_layer = new LauncherLayer();
		getWindow().show();
		pushLayer(m_layer);
	}
	LauncherApp::~LauncherApp() {
		
	}
}