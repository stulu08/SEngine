#pragma once
#include "Stulu/Layer.h"
namespace Stulu {
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onImguiRender() override;

		void Begin();
		void End();
	private:

		float m_time = 0.0f;
	};
}

