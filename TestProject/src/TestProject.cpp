#include <Stulu.h>
#include "imgui/imgui.h"

class testLayer : public Stulu::Layer {
public:
	testLayer()
		:Layer("Test"){}

	void onUpdate() override {
		
	}
	void onEvent(Stulu::Event& e) override {
		
	}
	void onImguiRender() override {

	}
};

class TestProject : public Stulu::Application {
public:
	TestProject() {
		pushLayer(new testLayer());
	}
	~TestProject() {
		
	}
};

Stulu::Application* Stulu::CreateApplication() {
	return new TestProject();
}