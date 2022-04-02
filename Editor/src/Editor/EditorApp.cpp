#include "EditorApp.h"
#include <Stulu/Core/EntryPoint.h>
#include "DiscordRPC.h"

namespace Stulu {
		
	Application* Stulu::CreateApplication() {
		return new EditorApp();
	}
	EditorApp* EditorApp::s_instance = nullptr;
	Project EditorApp::s_project;
	EditorApp::EditorApp() 
		: Application("Stulu Editor", true) {
		if (getStartArgs().size() > 1) {
			ST_INFO("Loading project: {0}", getStartArgs()[1]);
			s_project = Project(getStartArgs()[1]);
		}
		else {
			ST_ERROR("No Project, please open a Project");
			s_project = Project(Platform::browseFolder());
		}

		getWindow().setVSync(false);
		getWindow().setWindowIcon("Stulu/assets/Textures/Logo/engine-app-icon.png");
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		getWindow().setWindowTitle(std::string(ST_ENGINE_NAME) + " V" + ST_ENGINE_VERSION.to_string() + " - " + apiInfos.name + " " + apiInfos.version + " - " + apiInfos.device + " - " + s_project.path);
		s_instance = this;
		editorLayer = new EditorLayer();

		const char* rpcID = "733733383645167637";
#if ST_DEBUG
		if (strcmp(rpcID,"")) {
			//CORE_ASSERT(false, "Dont publish your id to the internet");
		}
#endif
		DiscordRPC::init(rpcID);
		DiscordRPC::setDetails("Loading the assets...");
		DiscordRPC::setState("");

		ST_INFO("Loading all Project assets from: {0}", s_project.assetPath);
		AssetsManager::loadAllFiles(s_project.assetPath);
		pushLayer(editorLayer);

		createMono(s_project.name);

		getWindow().show();
	}
	EditorApp::~EditorApp() {
		getEditorLayer().savePanelConfig(); 
		StyleEditor::saveAll();
		DiscordRPC::shutdown();
	}
}