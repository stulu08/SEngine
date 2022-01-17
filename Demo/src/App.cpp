#include "App.h"
#include <Stulu/Core/EntryPoint.h>

#include "GameLayer.h"

Stulu::Application* Stulu::CreateApplication() {
	return new Demo::App();
}

namespace Demo {
	App* App::s_instance = nullptr;
	App::App() :
		Application(ApplicationInfo("Stulu Engine 3D Demo", Version(1, 0, 0))) {
		s_instance = this;
		API_Infos apiInfos = getWindow().getContext()->getApiInfos();
		getWindow().setWindowTitle(std::string("Stulu Engine 3D Demo - ") + apiInfos.name + " " + apiInfos.version + " - " + apiInfos.device);

		m_gameLayer = new GameLayer();
		pushLayer(m_gameLayer);
		getWindow().show();
	}
	App::~App() {

	}
	Game& App::getGame() {
		return m_gameLayer->m_game;
	}
	Ref<Scene>& App::getGameScene() {
		return m_gameLayer->m_game.getGameScene();
	}
}