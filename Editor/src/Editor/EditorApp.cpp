#include "EditorApp.h"
#include <Stulu.h>
#include <Stulu/Core/EntryPoint.h>
namespace Stulu {
	EditorApp* EditorApp::s_instance = nullptr;
	Application* Stulu::CreateApplication() {
		return new EditorApp();
	}
}