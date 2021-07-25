#include <Stulu.h>
#include <Stulu/Input.h>

class testLayer : public Stulu::Layer {
public:
	testLayer()
		:Layer("Test"){}

	void onUpdate() override {
		if (Stulu::Input::isMouseDown(1)) {
			auto [x, y] = Stulu::Input::getMousePos();
			ST_INFO("x: {0}, y: {1}", x, y);
		}
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