#include "EditorApp.h"
#include <Stulu/Core/EntryPoint.h>
#include "DiscordRPC.h"
#include <StuluPremake.h>

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
		: Application(ApplicationInfo("Stulu Editor",Version(1,0,0), "Stulu", false, true, true, WindowProps("Stulu Editor", 1280, 720))) {
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
		getWindow().setWindowIcon("Editor/Textures/PNG - Icon - White.png");
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
		s_project.buildAssembly(s_project.dataPath + "/EditorProjectAssembly.dll");
		m_assembly = createRef<AssemblyManager>(s_project.dataPath + "/EditorProjectAssembly.dll", "Data/Managed/Stulu.ScriptCore.dll");
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


	void Project::generateProjectFiles() {
		ST_PROFILING_FUNCTION();
		stulu_premake_exec(path.c_str(), ST_PREMAKE_ACTION_VS2022);
	}

	void Project::rebuildAssembly() {
		ST_PROFILING_FUNCTION();
		generateProjectFiles();
#ifdef ST_PLATFORM_WINDOWS
		static const std::string buildCmd = "tools\\msbuild.bat \"" + path + "/Assembly.sln\"";

		static std::function<bool(const std::string&)> recompileFinished = [=](const std::string&)->bool {
			return std::filesystem::exists(dataPath + "/EditorProjectAssembly.dll");
		};
		static std::function<bool(const std::string&)> recompile = [=](const std::string&)->bool {
			return system(buildCmd.c_str());
		};
		assembly->reload(recompileFinished, recompile);
#else
		//will soon switch to mono xbuild for linux
		CORE_ERROR("Building Projects is not supported currently on this Platform");
#endif
		assembly->getClasses().clear();
		MonoClass* componentClass = Application::get().getAssemblyManager()->getScriptCoreAssembly()->createClass("Stulu", "Component");
		assembly->loadAllClasses(componentClass);
	}
	void Project::buildAssembly(const std::string& m_assembly) {
		ST_PROFILING_FUNCTION();
		generateProjectFiles();
#ifdef ST_PLATFORM_WINDOWS
		static const std::string buildCmd = "tools\\msbuild.bat \"" + path + "/Assembly.sln\"";

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