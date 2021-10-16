#include <Stulu.h>
#include <Stulu/Core/EntryPoint.h>


class App : public Stulu::Application {
public:
	App() 
		:Application(Stulu::ApplicationInfo("Vulkan testing", Version(1,1,1))) {
		getWindow().setVSysnc(false);
	}
	~App() {

	}
};

Stulu::Application* Stulu::CreateApplication() {
	return new App();
}