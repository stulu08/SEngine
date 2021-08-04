#pragma once
#include <Stulu.h>
#include <Layers/DefaultLayer.h>

class TestProject : public Stulu::Application {
public:
	TestProject() {
		pushLayer(defaultLayer);

		getWindow().setWindowIcon("assets/Logo/engine-icon-bg.png");
	}
	~TestProject() {

	}

	DefaultLayer* defaultLayer = new DefaultLayer();
};

