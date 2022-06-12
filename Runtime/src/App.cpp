#include "App.h"
#include <Stulu/Core/EntryPoint.h>

#include "Splash.h"

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
		YAML::Node node = YAML::LoadFile("Stulu/app");
		std::string name = node["App Name"].as<std::string>();
		Version version = *(Version*)&((glm::uvec3)node["App Verison"].as<glm::vec3>());
		WindowProps window;
		window.title = node["Window Title"].as<std::string>();
		window.width = node["Window Width"].as<uint32_t>();
		window.height = node["Window Height"].as<uint32_t>();
		window.VSync = node["Window VSync"].as<bool>();

		return new RuntimeApp(ApplicationInfo(name.c_str(), version, window));
	}
	RuntimeApp* RuntimeApp::s_instance = nullptr;

	RuntimeLayer* runtimeLayer;

	RuntimeApp::RuntimeApp(const ApplicationInfo& appInfo) 
		: Application(appInfo, false, false, "Stulu") {

		getWindow().setVSync(appInfo.windowProps.VSync);
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		s_instance = this;
		runtimeLayer = new RuntimeLayer();

		/* 
		{
			float m_lastFrameTime = Platform::getTime();
			Splash splashr = Splash(10.0f);
			while (true) {
				float time = Platform::getTime();
				Timestep delta = time - m_lastFrameTime;
				m_lastFrameTime = time;

				if (splashr.onUpdate(delta))
					break;

				getWindow().onUpdate();
			}
		}
		*/



		if (std::filesystem::exists(std::string(appInfo.ApplicationName) + "-data/Managed/Stulu.ScriptCore.dll"))
			std::filesystem::remove(std::string(appInfo.ApplicationName) + "-data/Managed/Stulu.ScriptCore.dll");
		std::filesystem::copy("data/Managed/Stulu.ScriptCore.dll", std::string(appInfo.ApplicationName) + "-data/Managed/Stulu.ScriptCore.dll");
		
		m_assembly = createRef<AssemblyManager>(std::string(appInfo.ApplicationName) + "-data/Managed/ProjectAssembly.dll", "data/Managed/Stulu.ScriptCore.dll");

		AssetsManager::loadAllFiles(std::string(appInfo.ApplicationName) + "-data/assets");



		getWindow().show();

		pushLayer(runtimeLayer);


	}
	RuntimeApp::~RuntimeApp() {

	}
}