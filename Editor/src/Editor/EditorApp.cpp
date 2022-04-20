#include "EditorApp.h"
#include <Stulu/Core/EntryPoint.h>
#include "DiscordRPC.h"
namespace Stulu {
	void loadEditorMonoBindings();
		
	Application* Stulu::CreateApplication() {
		return new EditorApp();
	}
	EditorApp* EditorApp::s_instance = nullptr;
	Project EditorApp::s_project;
	EditorApp::EditorApp() 
		: Application("Stulu Editor", true) {

		const char* rpcID = "733733383645167637";
#if ST_DEBUG
		if (strcmp(rpcID, "")) {
			//CORE_ASSERT(false, "Dont publish your id to the internet");
		}
#endif
		DiscordRPC::init(rpcID);
		DiscordRPC::setDetails("Loading the assets...");
		DiscordRPC::setState("");

		if (getStartArgs().size() > 1) {
			ST_INFO("Loading project: {0}", getStartArgs()[1]);
			s_project = Project(getStartArgs()[1]);
		}
		else {
			ST_ERROR("No Project, please open a Project");
			s_project = Project(Platform::browseFolder());
		}

		getWindow().setVSync(false);
		getWindow().setWindowIcon("assets/Textures/Logo/engine-app-icon.png");
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		getWindow().setWindowTitle(std::string(ST_ENGINE_NAME) + " V" + ST_ENGINE_VERSION.to_string() + " - " + apiInfos.name + " " + apiInfos.version + " - " + apiInfos.device + " - " + s_project.path);
		s_instance = this;
		editorLayer = new EditorLayer();


		//copy assembly files to project
		for (std::filesystem::directory_entry dir : std::filesystem::directory_iterator("data/Managed")) {
			if (dir.path().extension().string() == ".dll") {
				if (std::filesystem::exists(s_project.dataPath + "/" + dir.path().filename().string())) {
					std::filesystem::remove(s_project.dataPath + "/" + dir.path().filename().string());
				}
				std::filesystem::copy_file(dir.path(), s_project.dataPath + "/" + dir.path().filename().string());
			}
		}
		//m_assembly = createRef<AssemblyManager>(s_project.dataPath + "/ProjectAssembly.dll");
		m_assembly = createRef<AssemblyManager>(s_project.dataPath + "/EdiorProjectAssembly.dll", "data/Managed/Stulu.ScriptCore.dll");
		s_project.assembly = getAssemblyManager()->getAssembly();

		loadEditorMonoBindings();
		rebuildAssembly();
		

		ST_INFO("Loading all Project assets from: {0}", s_project.assetPath);
		AssetsManager::loadAllFiles(s_project.assetPath);
		pushLayer(editorLayer);


		getWindow().show();
	}
	EditorApp::~EditorApp() {
		s_project.assembly = nullptr;
		getEditorLayer().savePanelConfig(); 
		StyleEditor::saveAll();
	}
	void rebuildAssembly() {
#ifdef ST_PLATFORM_WINDOWS
		static const std::string buildCmd = "tools\\msbuild.bat " + getEditorProject().path;

		static std::function<bool(const std::string&)> recompileFinished = [=](const std::string&)->bool {
			return std::filesystem::exists(getEditorProject().dataPath + "/EdiorProjectAssembly.dll");
		};
		static std::function<bool(const std::string&)> recompile = [=](const std::string&)->bool {
			return system(buildCmd.c_str());
		};
		getEditorProject().assembly->reload(recompileFinished, recompile);
#else
		//will soon switch to mono xbuild for linux
		CORE_ERROR("Building Projects is not supported currently on this Platform");
#endif
	}

}