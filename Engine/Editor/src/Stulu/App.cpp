#include "App.h"
#include "Resources.h"
#include "Project.h"

#include <Stulu/Core/EntryPoint.h>

Editor::Project FindProject(int argc, char** argv) {
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

	return path;
}

Stulu::Application* Stulu::CreateApplication(int argc, char** argv) {
	Editor::Project project = FindProject(argc, argv);

	Log::AddFileSink(project.GetPath() + "/Logs/" + Log::generateTimeString() + ".log");
	ST_INFO("Loading project: {0}", project.GetPath());

	project.Compile(Editor::EditorBuildSettings);

	ApplicationInfo info;
	info.Name = "Stulu Editor";
	info.Publisher = "Stulu";
	info.Version = ST_ENGINE_VERSION;
	info.WindowProps.width = 1280;
	info.WindowProps.height = 720;
	info.WindowProps.VSync = false;
	info.DataPath = "Data";
	info.AppPath = project.GetPath();;
	info.AppAssetPath = project.GetAssetPath();;
	info.AppCachePath = project.GetCachePath();
	info.AppManagedAssembly = project.GetBinariesPath() + "/Managed/ManagedAssembly.dll";
	info.AppNativeAssembly = project.GetBinariesPath() + "/Native/NativeAssembly.dll";
	info.HideWindowOnSart = false;
	info.EnableImgui = true;
	info.LoadDefaultAssets = true;

	return new Editor::App(info, std::move(project));
}

namespace Editor {
	App* App::s_instance = nullptr;


	App::App(const ApplicationInfo& info, Project&& project)
		: Application(info), m_project(std::move(project)) {
		s_instance = this;

		getWindow().setWindowIcon(Editor::Resources::GetLogo()->getPath());
		getWindow().setWindowTitle(std::string(ST_ENGINE_NAME) + " V" + ST_ENGINE_VERSION_STR + " - " + m_project.GetPath());

		ST_INFO("Loading all Project assets from: {0}", m_project.GetAssetPath());

		AssetsManager::setProgessCallback(Application::LoadingScreen);
		AssetsManager::loadAllFiles(m_project.GetAssetPath());
		AssetsManager::setProgessCallback(nullptr);

		m_layer = new MainLayer();
		pushLayer(m_layer);
	}
	App::~App() {
		
	}
}