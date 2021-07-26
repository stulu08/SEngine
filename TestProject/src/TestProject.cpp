#include <Stulu.h>
#include "imgui/imgui.h"

class testLayer : public Stulu::Layer {
public:
	testLayer()
		:Layer("Test") {
		app = &Stulu::Application::get();
	}

	void onUpdate() override {
		
	}
	void onEvent(Stulu::Event& e) override {
		
	}
	void drawRendererInfos() {
		ImGui::Begin(std::string(app->graphicDriverConstants.vendor + " Renderer").c_str(), NULL, ImGuiWindowFlags_NoResize);
		ImGui::Text(app->graphicDriverConstants.renderer.c_str());
		ImGui::Text("Version %s", app->graphicDriverConstants.version.c_str());
		ImGui::Text("GLSL version %s", app->graphicDriverConstants.other.c_str());
		ImGui::End();

	}
	void onImguiRender() override {
		drawRendererInfos();

	}
private:
	Stulu::Application* app;
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