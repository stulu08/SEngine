#include <Stulu.h>
#include <Stulu/Core/EntryPoint.h>


class App : public Stulu::Application {
public:
	App() 
		:Application("Vulkan") {
		getWindow().setVSysnc(false);
	}
	~App() {

	}
};

Stulu::Application* Stulu::CreateApplication() {
	return new App();
}