#include "TestProject.h"
#include <Stulu/Core/EntryPoint.h>
#include "Layers/LightCube.h"
#include "Layers/Terrain.h"
#include "Layers/ModelLayer.h"
#include "Layers/Layer2D.h"

Stulu::Application* Stulu::CreateApplication() {
	return new TestProject();
}

TestProject::TestProject() {
	getWindow().setWindowIcon("assets/Logo/engine-icon-bg.png");
	getWindow().setVSysnc(false);

	//pushLayer(new LightCube());
	//pushLayer(new ModelLayer());
	//pushLayer(new TerrainLayer());
	pushLayer(new Layer2D());
}
