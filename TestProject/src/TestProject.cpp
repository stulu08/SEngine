#include <Stulu.h>
 

class testLayer : public Stulu::Layer {
public:
	testLayer()
		:Layer("Test"){}

	void onUpdate() override {
		ST_INFO("Testlayer::update");
	}
	void onEvent(Stulu::Event& e) override {
		ST_TRACE("{0}", e);
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