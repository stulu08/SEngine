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
		: Application("Stulu Editor") {
		if (getStartArgs().size() > 1) {
			ST_INFO("Loading project: {0}", getStartArgs()[1]);
			s_project = Project(getStartArgs()[1]);
		}
		else {
			ST_INFO("No Project, please open a Project");
			s_project = Project(Platform::browseFolder());
		}

		getWindow().setVSync(false);
		getWindow().setWindowIcon("Stulu/assets/Textures/Logo/engine-app-icon.png");
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		getWindow().setWindowTitle(std::string("Stulu Editor - ") + apiInfos.name + " " + apiInfos.version + " - " + apiInfos.device + " - " + s_project.path);
		s_instance = this;
		editorLayer = new EditorLayer();
		pushLayer(editorLayer);

		const char* rpcID = "";
#if ST_DEBUG
		if (strcmp(rpcID,"")) {
			//CORE_ASSERT(false, "Dont publish your id to the internet");
		}
#endif
		DiscordRPC::init(rpcID);



		CORE_INFO("Loading assets");
		AssetsManager::loadAllFiles(s_project.assetPath);
	}
	EditorApp::~EditorApp() {
		DiscordRPC::shutdown();
	}
}