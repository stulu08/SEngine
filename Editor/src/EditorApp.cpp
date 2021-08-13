#include "EditorApp.h"
#include <Stulu.h>
#include <Stulu/Core/EntryPoint.h>
EditorApp* EditorApp::s_instance = nullptr;
Stulu::Application* Stulu::CreateApplication() {
	return new EditorApp();
}