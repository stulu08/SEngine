#include "EditorApp.h"
#include <Stulu/Core/EntryPoint.h>
#include "DiscordRPC.h"

namespace Stulu {
	void loadEditorMonoBindings();
	Application* Stulu::CreateApplication() {
		ST_PROFILING_FUNCTION();
		uint64_t rp = 733733383645167637;
		DiscordRPC::init(std::to_string(rp).c_str());
		DiscordRPC::setDetails("Loading the assets...");
		DiscordRPC::setState("");
		return new EditorApp();
	}
	EditorApp* EditorApp::s_instance = nullptr;
	Project EditorApp::s_project;
	EditorApp::EditorApp() 
		: Application(ApplicationInfo("Stulu Editor", "Stulu", Version(1, 0, 0), "Data/Stulu", false, true, true, WindowProps("Stulu Editor", 1280, 720))) {
		ST_PROFILING_FUNCTION();
		
		if (getStartArgs().size() > 1) {
			ST_INFO("Loading project: {0}", getStartArgs()[1]);
			s_project = Project(getStartArgs()[1], true);
		}
		else {
			ST_ERROR("No Project, please open a Project");
			s_project = Project(Platform::openFile("Stulu Project File\0 * .sproj\0"), true);
		}

		Log::addFileSink(s_project.path + "/logs/" + Log::generateTimeString() + ".log");
		Resources::GameAssetDirectory = s_project.assetPath;

		getWindow().setVSync(false);
		getWindow().setWindowIcon(getDataPath() + "/Textures/PNG - Icon - White.png");
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		getWindow().setWindowTitle(std::string(ST_ENGINE_NAME) + " V" + ST_ENGINE_VERSION_STR + " - " + apiInfos.name + " " + apiInfos.version + " - " + apiInfos.device + " - " + s_project.path);
		s_instance = this;

		editorLayer = new EditorLayer();

		s_project.buildAssembly(s_project.dataPath + "/ManagedAssembly.dll");
		m_assembly = createRef<AssemblyManager>(s_project.dataPath + "/ManagedAssembly.dll", "Data/Stulu/Managed/Stulu.ScriptCore.dll");
		s_project.assembly = getAssemblyManager()->getAssembly();

		ST_INFO("Loading all Project assets from: {0}", s_project.assetPath);
		AssetsManager::loadAllFiles(s_project.assetPath);
		pushLayer(editorLayer);


		getWindow().show();
	}
	EditorApp::~EditorApp() {
		ST_PROFILING_FUNCTION();
		s_project.assembly = nullptr;
		getEditorLayer().savePanelConfig(); 
		StyleEditor::saveAll();
	}
}