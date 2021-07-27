#pragma once
#include "Stulu/Events/Event.h"

namespace Stulu {
	class KeyEvent : public Event {
	public:
		inline int getKeyCode() const {
			return m_keyCode;
		}

		EVENT_CLASS_CATEGORY(KeyboardEventCategrory | InputEventCategrory)

	protected:
		KeyEvent(int keycode)
			: m_keyCode(keycode){}

		int m_keyCode;
	};

	class KeyDownEvent : public KeyEvent {
	public:
		KeyDownEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), m_repeatCount(repeatCount){}

		inline int getRepeatCount() const { return m_repeatCount; }

		std::string toString() const override {
			std::stringstream s;
			s << "KeyDownEvent(" << m_keyCode << ":" << m_repeatCount << ")";
			return s.str();
		}

		EVENT_CLASS_TYPE(KeyDown)
	private:
		int m_repeatCount;
	};

	class KeyUpEvent : public KeyEvent {
	public:
		KeyUpEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string toString() const override {
			std::stringstream s;
			s << "KeyUpEvent(" << m_keyCode << ")";
			return s.str();
		}

		EVENT_CLASS_TYPE(KeyUp)
	};
	class KeyTypedEvent : public KeyEvent {
	public:
		KeyTypedEvent(int keycode)
			: KeyEvent(keycode) {}


		std::string toString() const override {
			std::stringstream s;
			s << "KeyDownEvent(" << m_keyCode << ")";
			return s.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}
