#pragma once
#include "Stulu/Layer.h"
#include <Stulu/Events/MouseEvent.h>
#include <Stulu/Events/ApplicationEvent.h>
#include <Stulu/Events/KeyEvent.h>
namespace Stulu {
	class STULU_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void onAttach();
		void onDetach();
		void onUpdate();
		void onEvent(Event& e);
	private:
		float m_time = 0.0f;

		bool onMouseButtonDownEvent(MouseButtonDownEvent& e);
		bool onMouseButtonUpEvent(MouseButtonUpEvent& e);
		bool onMouseMoveEvent(MouseMoveEvent& e);
		bool onMouseScrollEvent(MouseScrollEvent& e);

		bool onKeyTypedEvent(KeyTypedEvent& e);
		bool onKeyDownEvent(KeyDownEvent& e);
		bool onKeyUpEvent(KeyUpEvent& e);

		bool onWindowResizeEvent(WindowResizeEvent& e);
	};
}

