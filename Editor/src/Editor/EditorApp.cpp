#include "EditorApp.h"
#include <Stulu/Core/EntryPoint.h>
#include "DiscordRPC.h"

namespace Stulu {
		
	Application* Stulu::CreateApplication() {
		return new EditorApp();
	}
	EditorApp* EditorApp::s_instance = nullptr;
	EditorApp::EditorApp() 
		: Application("Stulu Editor") {
		getWindow().setVSync(false);
		getWindow().setWindowIcon("Stulu/assets/Textures/Logo/engine-app-icon.png");
		Resources::load();
		s_instance = this;
		editorLayer = new EditorLayer();
		pushLayer(editorLayer);

		const char* rpcID = "";

		if (strcmp(rpcID,"")) {
			CORE_ASSERT(false, "Dont publish your id to the internet");
			DiscordRPC::init(rpcID);
		}
	}
	EditorApp::~EditorApp() {
		DiscordRPC::shutdown();
	}
}