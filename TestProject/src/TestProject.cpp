#include "TestProject.h"
#include <Stulu/Core/EntryPoint.h>
#include "Layers/Layer2D.h"
#include "Layers/StuluEngine.h"


Stulu::Application* Stulu::CreateApplication() {
	return new TestProject();
}

TestProject::TestProject() {

	getWindow().setWindowIcon("Stulu/assets/Textures/Logo/engine-icon-bg.png");
	getWindow().setVSysnc(false);
	Stulu::RenderCommand::setClearColor(COLOR_BLACK);


#ifdef ST_DIST
	pushLayer(new Intro([](Stulu::Application* app) { app->pushLayer(new Layer2D()); }, this));
#else
	pushLayer(new Layer2D());
#endif //  ST_DIST

}

TestProject::~TestProject() {
}
