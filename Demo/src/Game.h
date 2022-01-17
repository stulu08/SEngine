#pragma once
#include "Player.h"

namespace Demo {
	class Game {
	public:
		Game();
		void initilize();
		void start();
		void stop();
		void update(Timestep t);

		void drawFrameBuffer();

		Ref<Scene>& getGameScene() {
			return m_scene;
		}
	private:
		Ref<Scene> m_scene = nullptr;
		Ref<Camera> m_playerCam = nullptr;
		GameObject m_playerObject;
		GameObject m_light;
		GameObject m_world;

		friend class GameLayer;
	};
}