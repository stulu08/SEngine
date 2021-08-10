#pragma once
#include <Stulu.h>
#include "Layers/LightCube.h"
#include "Layers/Terrain.h"
#include "Layers/Model.h"

class TestProject : public Stulu::Application {
public:
	TestProject() {
		getWindow().setWindowIcon("assets/Logo/engine-icon-bg.png");
		getWindow().setVSysnc(false);

		//pushLayer(new LightCube());
		pushLayer(new ModelLayer());
		//pushLayer(new TerrainLayer());



	};
};

