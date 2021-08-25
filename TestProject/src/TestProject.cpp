#include "TestProject.h"
#include <Stulu/Core/EntryPoint.h>
#include "Layers/Layer2D.h"


Stulu::Application* Stulu::CreateApplication() {
	return new TestProject();
}

TestProject::TestProject() {

	getWindow().setWindowIcon("Stulu/assets/Textures/Logo/engine-icon-bg.png");
	getWindow().setVSysnc(false);
	Stulu::RenderCommand::setClearColor(COLOR_GRAY);

	pushLayer(new Layer2D());
}

TestProject::~TestProject() {
}
