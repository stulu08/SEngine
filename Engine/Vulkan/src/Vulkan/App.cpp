#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#include <Stulu.h>

#include <Stulu/Core/EntryPoint.h>

class VulkanLayer : public Stulu::Layer {
public:
	void onAttach() override {
		Stulu::Renderer::init();
	}

	void onUpdate(Stulu::Timestep timestep) override {
		Stulu::RenderCommand::setClearColor({ 0.0f, 0.0f, 1.0f, 1.0f });
		Stulu::RenderCommand::clear();
	}
};

class VulkanApp : public Stulu::Application {
public:
	VulkanApp(const Stulu::ApplicationInfo& info) 
		: Application(info) {
		pushLayer(new VulkanLayer());
	}
};

namespace Stulu {
	Application* CreateApplication(int argc, char** argv) {
		ApplicationInfo info;
		info.DataPath = "Data";

		info.api = Renderer::API::Vulkan;
		info.EnableImgui = false;
		info.LoadDefaultAssets = false;
		info.HideWindowOnSart = false;

		return new VulkanApp(info);
	}
}