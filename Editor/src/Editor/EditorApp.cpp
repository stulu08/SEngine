#include "EditorApp.h"
#include <Stulu.h>
#include <Stulu/Core/EntryPoint.h>
namespace Stulu {
	Application* Stulu::CreateApplication() {
		return new EditorApp();
	}
	EditorApp* EditorApp::s_instance = nullptr;
	EditorApp::EditorApp() 
		: Application("Stulu Editor") {

		getWindow().setWindowIcon("Stulu/assets/Textures/Logo/engine-app-icon.png");
		s_instance = this;
		editorLayer = new EditorLayer();
		pushLayer(editorLayer);
	}
}