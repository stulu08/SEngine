#pragma once
#include "Game.h"

namespace Demo {
	class GameLayer : public Layer {
	public:
		GameLayer();
		~GameLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onUpdate(Timestep t) override;
		virtual void onImguiRender(Timestep t) override;
		virtual void onEvent(Event& e) override;

	private:
		Game m_game;

		bool onWindowResize(WindowResizeEvent& e);

		friend class App;
	};
}