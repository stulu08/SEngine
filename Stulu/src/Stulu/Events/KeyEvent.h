#pragma once
#include "Event.h"

namespace Stulu {
	class STULU_API KeyEvent : public Event {
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

	class STULU_API KeyDownEvent : public KeyEvent {
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

	class STULU_API KeyUpEvent : public KeyEvent {
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
	class STULU_API KeyTypedEvent : public KeyEvent {
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
