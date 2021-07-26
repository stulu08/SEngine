#pragma once
#include "Stulu/Layer.h"
#include "Stulu/Events/MouseEvent.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Events/KeyEvent.h"
namespace Stulu {
	class STULU_API ImGuiLayer : public Layer
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

