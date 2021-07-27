#pragma once
#include <Stulu.h>
#include <Layers/DefaultLayer.h>

class TestProject : public Stulu::Application {
public:
	TestProject() {
		pushLayer(defaultLayer);
		getWindow().setVSysnc(false);
	}
	~TestProject() {

	}

	DefaultLayer* defaultLayer = new DefaultLayer();
};

