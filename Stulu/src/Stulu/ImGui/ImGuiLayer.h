#pragma once
#include "Stulu/Core/Layer.h"
struct ImGuiStyle;
struct ImVec4;
namespace Stulu {
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onImguiRender(Timestep timestep) override;
		virtual void onEvent(Event& e) override;
		void blockEvents(bool blockEvents) { m_blockEvents = blockEvents; }

		void Begin();
		void End();
		void StyleColorsAmoledDark(ImGuiStyle* dst = NULL);
		void StyleColorsOceanDark(ImGuiStyle* dst = NULL);

	private:
		bool m_blockEvents = true;
		float m_time = 0.0f;
	};
}

