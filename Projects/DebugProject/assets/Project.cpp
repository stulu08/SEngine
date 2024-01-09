#include <Stulu.h>

#include "Component/FreeCamera.h"

class MyProject : Stulu::Layer {
public:
	virtual void onAttach() override {
		auto& assembly = Stulu::Application::get().getAssemblyManager();

		assembly->RegisterComponent<FreeCamera>("FreeCameraComponent");
	}
	virtual void onUpdate(Stulu::Timestep ts) override {
		
	}
	virtual void onEvent(Stulu::Event& e) override {
		
	}
private:
};