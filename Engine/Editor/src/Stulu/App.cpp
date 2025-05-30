#include "App.h"
#include "Resources.h"
#include "Project.h"

#include <Stulu/Core/EntryPoint.h>

#include "Bindings/Bindings.h"
#include "MainLayer.h"

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
	Editor::Prefrences prefrences = project.GetConfigPath() + "/Prefrences.ini";

	Log::AddFileSink(project.GetPath() + "/Logs/" + Log::generateTimeString() + ".log");
	ST_INFO("Loading project: {0}", project.GetPath());

	//project.Compile(Editor::EditorBuildSettings);
	project.Compile(Editor::ManagedOnlyBuildSettings);

	prefrences.Section("Window");
	ApplicationInfo info;
	info.Name = "Stulu Editor";
	info.Publisher = "Stulu";
	info.Version = ST_ENGINE_VERSION;
	info.WindowProps.width = 1280;
	info.WindowProps.height = 720;
	info.WindowProps.VSync = prefrences.GetAs("Vsync", true);
	info.DataPath = "Data";
	info.AppPath = project.GetPath();;
	info.AppAssetPath = project.GetAssetPath();;
	info.AppCachePath = project.GetCachePath();
	info.AppManagedAssembly = project.GetBinariesPath() + "/Managed/ManagedAssembly.dll";
	info.AppNativeAssembly = project.GetBinariesPath() + "/Native/NativeAssembly.dll";
	info.HideWindowOnSart = false;
	info.EnableImgui = true;
	info.StartPhysicsEngine = true;
	info.LoadDefaultAssets = true;
	info.DebugFeatures = Editor::StandartBuildConfig != Editor::BuildConfiguration::Dist;

	return new Editor::App(info, std::move(project), std::move(prefrences));
}

namespace Editor {
	App* App::s_instance = nullptr;


	App::App(const ApplicationInfo& info, Project&& project, Prefrences&& prefs)
		: Application(info), m_project(std::move(project)), m_prefrences(std::move(prefs)) {
		s_instance = this;
		LoadingScreen(0.0f);

		getWindow().setWindowIcon(Editor::Resources::GetLogo()->getPath());
		getWindow().setWindowTitle(std::string(ST_ENGINE_NAME) + " V" + ST_ENGINE_VERSION_STR + " - " + m_project.GetPath());

		Editor::LoadEditorBindings();
		Editor::LoadImGuiBindings();
		LoadingScreen(0.1f);

		m_layer = new MainLayer();
		pushLayer(m_layer);
		LoadingScreen(0.4f);

		ST_INFO("Loading all Project assets from: {0}", m_project.GetAssetPath());

		AssetsManager::GlobalInstance().LoadDirectoryRecursive(m_project.GetAssetPath());
		LoadingScreen(0.5f);

		m_layer->onLoadFinish();
	}
	App::~App() {
		Editor::Resources::ReleaseAll();
		m_prefrences.Save();
	}
}