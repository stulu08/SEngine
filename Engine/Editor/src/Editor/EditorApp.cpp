#include "EditorApp.h"
#include <Stulu/Core/EntryPoint.h>

namespace Stulu {
	Application* Stulu::CreateApplication(int argc, char** argv) {
		ST_PROFILING_FUNCTION();

		std::string path = "";
		if (argc > 1) {
			path = argv[1];
		}
		else {
			while (!std::filesystem::exists(path)) {
				path = Platform::browseFolder();
				ST_INFO("Project not found: {0}", path);
			}
		}
		CleanPath(path);
		if (path[path.size() - 1] == '/')
			path.pop_back();

		Log::AddFileSink(path + "/Logs/" + Log::generateTimeString() + ".log");
		
		ST_INFO("Loading project: {0}", path);
		Project::Main = createScope<Project>(path);
		Project::Main->compileAssembly();

		ApplicationInfo info;
		info.Name = "Stulu Editor";
		info.Publisher = "Stulu";
		info.Version = ST_ENGINE_VERSION;
		info.WindowProps.width = 1280;
		info.WindowProps.height = 720;
		info.DataPath = "Data";
		info.AppPath = path;
		info.AppAssetPath = Project::Main->assetPath;
		info.AppAssembly = Project::Main->getBinariesDir() + "/ManagedAssembly.dll";
		info.HideWindowOnSart = false;
		info.EnableImgui = true;
		info.LoadDefaultAssets = true;
		return new EditorApp(info);
	}

	EditorApp* EditorApp::s_instance = nullptr;

	EditorApp::EditorApp(const ApplicationInfo& info)
		: Application(info) {
		ST_PROFILING_FUNCTION();
		s_instance = this;

		getWindow().setVSync(false);
		getWindow().setWindowIcon(getDataPath() + "/Textures/PNG - Icon - White.png");
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		getWindow().setWindowTitle(std::string(ST_ENGINE_NAME) + " V" + ST_ENGINE_VERSION_STR + " - " + apiInfos.name + " " + apiInfos.version + " - " + apiInfos.device + " - " + Project::Main->path);

		ST_INFO("Loading all Project assets from: {0}", Project::Main->assetPath);
		AssetsManager::loadAllFiles(Project::Main->assetPath);

		editorLayer = new EditorLayer();
		pushLayer(editorLayer);

		getWindow().show();
	}
	EditorApp::~EditorApp() {
		ST_PROFILING_FUNCTION();
		getEditorLayer().savePanelConfig(); 
		StyleEditor::saveAll();
	}
}