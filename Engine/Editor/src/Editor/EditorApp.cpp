#include "EditorApp.h"
#include <Stulu/Core/EntryPoint.h>
#include "DiscordRPC.h"
namespace Stulu {
	void loadEditorMonoBindings();
	Application* Stulu::CreateApplication() {
		uint64_t rp = 733733383645167637;
		DiscordRPC::init(std::to_string(rp).c_str());
		DiscordRPC::setDetails("Loading the assets...");
		DiscordRPC::setState("");
		return new EditorApp();
	}
	EditorApp* EditorApp::s_instance = nullptr;
	Project EditorApp::s_project;
	EditorApp::EditorApp() 
		: Application(ApplicationInfo("Stulu Editor",Version(1,0,0), "Stulu", false, true, WindowProps("Stulu Editor", 1280, 720))) {
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
		getWindow().setWindowIcon("Editor/Textures/engine-app-icon.png");
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		getWindow().setWindowTitle(std::string(ST_ENGINE_NAME) + " V" + ST_ENGINE_VERSION.to_string() + " - " + apiInfos.name + " " + apiInfos.version + " - " + apiInfos.device + " - " + s_project.path);
		s_instance = this;
		editorLayer = new EditorLayer();


		//copy assembly files to project
		for (std::filesystem::directory_entry dir : std::filesystem::directory_iterator("Data/Managed")) {
			if (dir.path().extension().string() == ".dll" || dir.path().extension().string() == ".xml"/*c# doc file*/) {
				if (!std::filesystem::exists(s_project.dataPath)) {
					std::filesystem::create_directory(s_project.dataPath);
				}
				if (std::filesystem::exists(s_project.dataPath + "/" + dir.path().filename().string())) {
					std::filesystem::remove(s_project.dataPath + "/" + dir.path().filename().string());
				}
				std::filesystem::copy_file(dir.path(), s_project.dataPath + "/" + dir.path().filename().string());
			}
		}
		buildAssembly(s_project.dataPath + "/EdiorProjectAssembly.dll");
		//m_assembly = createRef<AssemblyManager>(s_project.dataPath + "/ProjectAssembly.dll");
		m_assembly = createRef<AssemblyManager>(s_project.dataPath + "/EdiorProjectAssembly.dll", "Data/Managed/Stulu.ScriptCore.dll");
		s_project.assembly = getAssemblyManager()->getAssembly();

		loadEditorMonoBindings();


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



	void rebuildAssembly() {
		ST_PROFILING_FUNCTION();
#ifdef ST_PLATFORM_WINDOWS
		static const std::string buildCmd = "tools\\msbuild.bat \"" + getEditorProject().path + "/Assembly.sln\"";

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
		getEditorProject().assembly->getClasses().clear();
		MonoClass* componentClass = Application::get().getAssemblyManager()->getScriptCoreAssembly()->createClass("Stulu", "Component");
		getEditorProject().assembly->loadAllClasses(componentClass);
	}
	void buildAssembly(const std::string& m_assembly) {
		ST_PROFILING_FUNCTION();
#ifdef ST_PLATFORM_WINDOWS
		static const std::string buildCmd = "tools\\msbuild.bat \"" + getEditorProject().path + "/Assembly.sln\"";

		if (std::filesystem::exists(m_assembly))
			std::filesystem::remove(m_assembly);

		CORE_INFO("Compiling {0}", m_assembly);
		if (system(buildCmd.c_str())) {
			CORE_ERROR("Compiling failed: {0}", m_assembly);
		}
		while (true) {
			if (std::filesystem::exists(m_assembly)) {
				if (std::filesystem::exists(m_assembly)) {
					CORE_INFO("Compiling finished");
					break;
				}
				else {
					CORE_ERROR("File not found: {0}", m_assembly);
					CORE_ERROR("Compiling failed: {0}", m_assembly);
					break;
				}

			}
		}
#else
		//will soon switch to mono xbuild for linux
		CORE_ERROR("Building Projects is not supported currently on this Platform");
#endif
	}

}