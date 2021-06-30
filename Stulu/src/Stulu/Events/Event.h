#pragma once

#include "Stulu/Core.h"

#include <string.h>
#include <functional>

namespace Stulu {
	enum EventCategory { none=0,
		ApplicationEventCategrory = ST_BIT(0),
		InputEventCategrory = ST_BIT(1),
		KeyboardEventCategrory = ST_BIT(2),
		MouseEventCategrory = ST_BIT(3),
		MouseButtonEventCategrory = ST_BIT(4),
	};
	enum class EvenType { none=0,
		//input
		KeyDown, KeyUp, MouseButtonDown, MouseButtonUp, MouseMove, MouseScroll,
		//Application
		ApplicationUpdate, ApplicationRender, ApplicationTick,
		//Window
		WindowMove, WindowResize, WindowFocus, WindowFocusLost, WindowClose
	};

	class STULU_API event
	{
		friend class EventDispatcher;
	public:
		virtual EvenType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool isInCategory(EventCategory category) {
			return GetCategoryFlags() & category;
		}
	protected:
		bool m_Handled = false;
	};
	class EventDispatcher {

	};
}