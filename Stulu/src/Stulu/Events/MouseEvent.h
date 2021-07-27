#pragma once
#include "Stulu/Events/Event.h"


namespace Stulu {
	class MouseMoveEvent : public Event {
	public:
		MouseMoveEvent(float x, float y)
			: m_mouseX(x), m_mouseY(y){}

		inline float getX() const { return m_mouseX; }
		inline float getY() const { return m_mouseY; }

		std::string toString() const override {
			std::stringstream s;
			s << "MouseMoveEvent(" << m_mouseX << ":" << m_mouseY << ")";
			return s.str();
		}

		EVENT_CLASS_TYPE(MouseMove)
		EVENT_CLASS_CATEGORY(MouseEventCategrory | InputEventCategrory)

	private:
		float m_mouseX, m_mouseY;
	};
	class MouseScrollEvent : public Event {
	public:
		MouseScrollEvent(float xOff, float yOff)
			: m_xOff(xOff), m_yOff(yOff){}

		inline float getXOff() const { return m_xOff; }
		inline float getYOff() const { return m_yOff; }

		std::string toString() const override {
			std::stringstream s;
			s << "MouseScrollEvent(" << m_xOff << ":" << m_yOff << ")";
			return s.str();
		}

		EVENT_CLASS_TYPE(MouseScroll)
		EVENT_CLASS_CATEGORY(MouseEventCategrory | InputEventCategrory)

	private:
		float m_xOff, m_yOff;
	};

	class MouseButtonEvent : public Event {
	public:
		inline int getButton() const { return m_button; }
		EVENT_CLASS_CATEGORY(MouseEventCategrory | InputEventCategrory)

	protected:
		MouseButtonEvent(int button)
			: m_button(button){}
		int m_button;
	};
	class MouseButtonDownEvent : public MouseButtonEvent{
	public:
		MouseButtonDownEvent(int button)
			: MouseButtonEvent(button){}

		std::string toString() const override {
			std::stringstream s;
			s << "MouseButtonDownEvent(" << m_button << ")";
			return s.str();
		}
		EVENT_CLASS_TYPE(MouseButtonDown)
	};
	class MouseButtonUpEvent : public MouseButtonEvent {
	public:
		MouseButtonUpEvent(int button)
			: MouseButtonEvent(button) {}

		std::string toString() const override {
			std::stringstream s;
			s << "MouseButtonUpEvent(" << m_button << ")";
			return s.str();
		}
		EVENT_CLASS_TYPE(MouseButtonUp)
	};
}