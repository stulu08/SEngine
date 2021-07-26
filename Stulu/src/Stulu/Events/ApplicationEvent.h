#pragma once
#include "Stulu/Events/Event.h"

namespace Stulu {
	class STULU_API WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_width(width), m_height(height){}

		inline unsigned int getWidth() const { return m_width; }
		inline unsigned int getHeight() const { return m_height; }

		std::string toString() const override {
			std::stringstream s;
			s << "WindowResizeEvent(" << m_width << ":" << m_height << ")";
			return s.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(ApplicationEventCategrory)

	private:
		unsigned int m_width, m_height;
	};

	class STULU_API WindowCloseEvent : public Event {
	public:
		WindowCloseEvent(){}
		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(ApplicationEventCategrory)
	};



	class STULU_API ApplicationUpdateEvent : public Event {
	public:
		ApplicationUpdateEvent(){}
		EVENT_CLASS_TYPE(ApplicationUpdate)
		EVENT_CLASS_CATEGORY(ApplicationEventCategrory)
	};	
	
	class STULU_API ApplicationRenderEvent : public Event {
	public:
		ApplicationRenderEvent(){}
		EVENT_CLASS_TYPE(ApplicationRender)
		EVENT_CLASS_CATEGORY(ApplicationEventCategrory)
	};	
	
	class STULU_API ApplicationTickEvent : public Event {
	public:
		ApplicationTickEvent() {}
		EVENT_CLASS_TYPE(ApplicationTick)
			EVENT_CLASS_CATEGORY(ApplicationEventCategrory)
		};
}