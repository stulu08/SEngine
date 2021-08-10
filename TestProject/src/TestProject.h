#pragma once
#include <Stulu.h>
#include "Layers/DefaultLayer.h"
#include "Layers/GameLayer.h"
#include "Layers/Terrain.h"

#include "DemoGame/DemoGameLayer.h"
#include "DemoGame/MenuLayer.h"

class TestProject : public Stulu::Application {
public:
	TestProject() {
		getWindow().setWindowIcon("assets/Logo/engine-icon-bg.png");
		getWindow().setVSysnc(false);

		//pushLayer(new GameLayer());
		//pushLayer(new DefaultLayer());
		pushLayer(new TerrainLayer());



		//m_menuLayer = new Menu();
		//pushLayer(m_menuLayer);
	}
	virtual void _onEvent(Stulu::Event& e) override {
		Stulu::EventDispatcher dis(e);
		//dis.dispatch<KeyDownEvent>(ST_BIND_EVENT_FN(TestProject::onKeyDown));
	}


	bool onKeyDown(Stulu::KeyDownEvent& e) {
		if (m_playing)
			return false;
		Application::get().pushLayer(new DemoGameLayer());
		Application::get().popLayer(m_menuLayer);
		m_playing = true;
		return false;
	}
private:
	Menu* m_menuLayer;
	bool m_playing = false;
};

