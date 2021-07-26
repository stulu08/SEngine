#include <Stulu.h>
#include <Stulu/EntryPoint.h>

#include "Layers/GuiLayer.h"
#include "Layers/DefaultLayer.h"

class TestProject : public Stulu::Application {
public:
	TestProject() {
		pushLayer(new DefaultLayer());
		pushOverlay(new GuiLayer());
	}
	~TestProject() {
		
	}
};

Stulu::Application* Stulu::CreateApplication() {
	return new TestProject();
}