#pragma once
#include <Stulu.h>
#include "Layers/DefaultLayer.h"
#include "Layers/GameLayer.h"
#include "Layers/DirectXLayer.h"

class TestProject : public Stulu::Application {
public:
	TestProject() {
		//pushLayer(new DirectXLayer());
		pushLayer(new GameLayer());
		//pushLayer(new DefaultLayer());
		
		getWindow().setWindowIcon("assets/Logo/engine-icon-bg.png");
	}
	~TestProject() {

	}

};

