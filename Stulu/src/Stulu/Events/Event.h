#pragma once

#include "st_pch.h"
#include "Stulu/Core/Core.h"


namespace Stulu {
	enum EventCategory { none=0,
		ApplicationEventCategrory = ST_BIT(0),
		InputEventCategrory = ST_BIT(1),
		KeyboardEventCategrory = ST_BIT(2),
		MouseEventCategrory = ST_BIT(3),
		MouseButtonEventCategrory = ST_BIT(4),
	};
	enum class EventType { none=0,
		//input
		KeyTyped, KeyDown, KeyUp, MouseButtonDown, MouseButtonUp, MouseMove, MouseScroll,
		//Application
		ApplicationUpdate, ApplicationRender, ApplicationTick,
		//Window
		WindowMove, WindowResize, WindowFocus, WindowFocusLost, WindowClose
	};
#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; }\
								virtual EventType getEventType() const override { return getStaticType(); }\
								virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

	class Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }

		inline bool isInCategory(EventCategory category) {
			return getCategoryFlags() & category;
		}
		bool handled = false;
	};
	class EventDispatcher {
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_event(event){}

		template<typename T>
		bool dispacth(EventFn<T> func) {
			if (m_event.getEventType() == T::getStaticType()) {
				m_event.handled = func(*(T*)&m_event);
				return true;
			}
			return false;
		}
	private:
		Event& m_event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e) {
		return os << e.toString();
	}
}