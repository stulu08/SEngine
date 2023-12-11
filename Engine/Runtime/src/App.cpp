#include "App.h"
#include <Stulu/Core/EntryPoint.h>

#include "Stulu/Scene/YAML.h"

namespace Stulu {
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
	Application* Stulu::CreateApplication() {
		ApplicationInfo appInfo;
		try {
			YAML::Node node = YAML::LoadFile("Stulu/app");
			if(node["App Name"])
				appInfo.ApplicationName = node["App Name"].as<std::string>();
			if(node["App Verison"])
				appInfo.ApplicationVersion = *(Version*)&((glm::uvec3)node["App Verison"].as<glm::vec3>());
			appInfo.DefaultAssetsPath = "Data/Stulu";
			appInfo.EnableImgui = false;
			appInfo.HideWindowOnSart = false;
			appInfo.LoadDefaultAssets = true;

			if (node["Window Title"])
				appInfo.windowProps.title = node["Window Title"].as<std::string>();
			if (node["Window Width"])
				appInfo.windowProps.width = node["Window Width"].as<uint32_t>();
			if (node["Window Height"])
				appInfo.windowProps.height = node["Window Height"].as<uint32_t>();
			if (node["Window VSync"])
				appInfo.windowProps.VSync = node["Window VSync"].as<bool>();

			if(node["debug"])
				RuntimeLayer::DebugMode = node["debug"].as<bool>();
		}
		catch (YAML::Exception ex) {
			CORE_ERROR("Error while retrieving app information:\n", ex.what());
		}
		
		return new RuntimeApp(appInfo);
	}
	RuntimeApp* RuntimeApp::s_instance = nullptr;

	RuntimeLayer* runtimeLayer;

	RuntimeApp::RuntimeApp(const ApplicationInfo& appInfo) 
		: Application(appInfo) {
		Log::addFileSink("logs/" + Log::generateTimeString() + ".log", Log::Level::warn);

		getWindow().setVSync(appInfo.windowProps.VSync);
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		Resources::GameAssetDirectory = std::string(appInfo.ApplicationName) + "-data/assets";
		s_instance = this;
		runtimeLayer = new RuntimeLayer();

		if (std::filesystem::exists(std::string(appInfo.ApplicationName) + "-data/Managed/Stulu.ScriptCore.dll"))
			std::filesystem::remove(std::string(appInfo.ApplicationName) + "-data/Managed/Stulu.ScriptCore.dll");
		std::filesystem::copy("data/Managed/Stulu.ScriptCore.dll", std::string(appInfo.ApplicationName) + "-data/Managed/Stulu.ScriptCore.dll");
		
		m_assembly = createRef<AssemblyManager>(std::string(appInfo.ApplicationName) + "-data/Managed/ProjectAssembly.dll", "Data/Managed/Stulu.ScriptCore.dll");

		AssetsManager::loadAllFiles(std::string(appInfo.ApplicationName) + "-data/assets");


		getWindow().show();

		pushLayer(runtimeLayer);


	}
	RuntimeApp::~RuntimeApp() {
		
	}
}