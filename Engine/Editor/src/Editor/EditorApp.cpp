#include "EditorApp.h"
#include <Stulu/Core/EntryPoint.h>

namespace Stulu {
	Application* Stulu::CreateApplication(int argc, char** argv) {
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
		info.AppCachePath = Project::Main->path + "/Compiler/";
		info.AppManagedAssembly = Project::Main->getBinariesDir() + "/Managed/ManagedAssembly.dll";
		info.AppNativeAssembly = Project::Main->getBinariesDir() + "/Native/NativeAssembly.dll";
		info.HideWindowOnSart = false;
		info.EnableImgui = true;
		info.LoadDefaultAssets = true;
		return new EditorApp(info);
	}

	EditorApp* EditorApp::s_instance = nullptr;

	EditorApp::EditorApp(const ApplicationInfo& info)
		: Application(info) {
		s_instance = this;

		getWindow().setVSync(false);
		getWindow().setWindowIcon(getDataPath() + "/Textures/PNG - Icon - White.png");
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		getWindow().setWindowTitle(std::string(ST_ENGINE_NAME) + " V" + ST_ENGINE_VERSION_STR + " - " + apiInfos.name + " " + apiInfos.version + " - " + apiInfos.device + " - " + Project::Main->path);

		ST_INFO("Loading all Project assets from: {0}", Project::Main->assetPath);
		AssetsManager::setProgessCallback(Application::LoadingScreen);
		AssetsManager::loadAllFiles(Project::Main->assetPath);
		AssetsManager::setProgessCallback(nullptr);

		editorLayer = new EditorLayer();
		pushLayer(editorLayer);

		getWindow().show();
	}
	EditorApp::~EditorApp() {
		getEditorLayer().savePanelConfig(); 
		StyleEditor::saveAll();
	}
}