#include <Stulu.h>


class testLayer : public Stulu::Layer {
public:
	testLayer()
		:Layer("Test"){}

	void onUpdate() override {
		
	}
	void onEvent(Stulu::Event& e) override {
		
	}
};

class TestProject : public Stulu::Application {
public:
	TestProject() {
		pushLayer(new testLayer());
		pushOverlay(new Stulu::ImGuiLayer());
	}
	~TestProject() {

	}
};

Stulu::Application* Stulu::CreateApplication() {
	return new TestProject();
}