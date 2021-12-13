#include "Layer.h"
#include <Stulu/Core/EntryPoint.h>


class App : public Stulu::Application {
public:
	App() 
		:Application(Stulu::ApplicationInfo("Vulkan testing", Version(1,1,1))) {
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		getWindow().setWindowTitle(std::string("Vulkan testing - ") + apiInfos.name + " " + apiInfos.version + " - " + apiInfos.device);
		pushLayer(new DefaultLayer());
	}
	~App() {

	}
};

Stulu::Application* Stulu::CreateApplication() {
	return new App();
}