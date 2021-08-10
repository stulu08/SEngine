#pragma once
#include <Stulu.h>
#include "Map.h"
#include "Player.h"

using namespace Stulu;

class DemoGameLayer : public Layer {
public:
	DemoGameLayer()
		:Layer("DemoGame") {
		app = &Application::get();
	}
	~DemoGameLayer() {
	}
	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onUpdate(Stulu::Timestep timestep) override;
	virtual void onImguiRender(Stulu::Timestep timestep) override;
	virtual void onEvent(Stulu::Event& e) override;
private:
	Application* app;
	Player player;
	Map m_map;
};

