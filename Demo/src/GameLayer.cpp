#include "GameLayer.h"
#include "Chunk.h"
#include "imgui/imgui.h"

namespace Demo {
	GameLayer::GameLayer() 
		:Layer("GameLayer") {

	}
	GameLayer::~GameLayer() {

	}
	void GameLayer::onAttach() {
		Resources::load();
		World::loadBlocks();
		Chunk::loadShader();
		m_game = Game();
		m_game.initilize();
		m_game.start();
	}
	void GameLayer::onDetach() {
		m_game.stop();
	}
	void GameLayer::onUpdate(Timestep t) {
		m_game.update(t);
	}
	void GameLayer::onImguiRender(Timestep t) {
		m_game.drawFrameBuffer();
	}
	void GameLayer::onEvent(Event& e) {
		EventDispatcher dis(e);
		dis.dispatch<WindowResizeEvent>(ST_BIND_EVENT_FN(GameLayer::onWindowResize));
	}
	bool GameLayer::onWindowResize(WindowResizeEvent& e) {
		m_game.getGameScene()->onViewportResize(e.getWidth(), e.getHeight());
		return true;
	}
}