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
#if 0
		std::string name("Stulu Runtime");
		Version version(1, 0, 0);
		WindowProps window;
		window.title = name;
		window.width = 1920;
		window.height = 1080;
		window.VSync = false;

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "App Name" << YAML::Value << name;
		out << YAML::Key << "App Verison" << YAML::Value << (glm::vec3) * ((glm::uvec3*)&version);
		out << YAML::Key << "Window Title" << YAML::Value << window.title;
		out << YAML::Key << "Window Width" << YAML::Value << window.width;
		out << YAML::Key << "Window Height" << YAML::Value << window.height;
		out << YAML::Key << "Window VSync" << YAML::Value << window.VSync;
		out << YAML::EndMap;


		FILE* file = fopen("data/app.sapp", "w");
		fprintf(file,out.c_str());
		fclose(file);
#else
		YAML::Node node = YAML::LoadFile("data/app.sapp");
		std::string name = node["App Name"].as<std::string>();
		Version version = *(Version*)&((glm::uvec3)node["App Verison"].as<glm::vec3>());
		WindowProps window;
		window.title = node["Window Title"].as<std::string>();
		window.width = node["Window Width"].as<uint32_t>();
		window.height = node["Window Height"].as<uint32_t>();
		window.VSync = node["Window VSync"].as<bool>();
#endif

		return new RuntimeApp(ApplicationInfo(name.c_str(), version, window));
	}
	RuntimeApp* RuntimeApp::s_instance = nullptr;
	RuntimeApp::RuntimeApp(const ApplicationInfo& appInfo) 
		: Application(appInfo, false, false, "Stulu") {

		getWindow().setVSync(appInfo.windowProps.VSync);
		getWindow().setWindowIcon("Stulu/Textures/Logo/engine-app-icon.png");
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		s_instance = this;
		editorLayer = new RuntimeLayer();

		m_assembly = createRef<AssemblyManager>("data/Managed/ProjectAssembly.dll", "data/Managed/Stulu.ScriptCore.dll");

		pushLayer(editorLayer);

		getWindow().show();
	}
	RuntimeApp::~RuntimeApp() {
		
	}
}