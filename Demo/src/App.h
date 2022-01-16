#pragma once
#include <Stulu.h>

using namespace Stulu;

#define getApp() App::get()
#define getScene() App::get().getGameScene()

namespace Demo {
	class GameLayer;
	class Game;
	class App : public Application{
	public:
		App();
		~App();

		static inline App& get() {
			return *s_instance;
		}
		Game& getGame();
		Ref<Scene>& getGameScene();
	private:
		GameLayer* m_gameLayer;

		static App* s_instance;
	};
}