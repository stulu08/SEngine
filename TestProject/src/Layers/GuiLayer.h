#pragma once
#include <Stulu.h>
#include "imgui/imgui.h"

class GuiLayer : public Stulu::Layer
{
public:
	GuiLayer()
		:Layer("Gui") {
		app = &Stulu::Application::get();
	}

	virtual void onUpdate() override;
	virtual void onEvent(Stulu::Event& e) override;
	void drawRendererInfos();
	void onImguiRender() override;

private:
	Stulu::Application* app;
};

