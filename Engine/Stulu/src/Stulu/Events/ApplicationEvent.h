#pragma once
#include "Stulu/Events/Event.h"

namespace Stulu {
	class WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			: m_width(width), m_height(height){}

		inline uint32_t getWidth() const { return m_width; }
		inline uint32_t getHeight() const { return m_height; }

		std::string toString() const override {
			std::stringstream s;
			s << "WindowResizeEvent(" << m_width << ":" << m_height << ")";
			return s.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(ApplicationEventCategrory)

	private:
		uint32_t m_width, m_height;
	};

	class WindowCloseEvent : public Event {
	public:
		WindowCloseEvent(){}
		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(ApplicationEventCategrory)
	};



	class ApplicationUpdateEvent : public Event {
	public:
		ApplicationUpdateEvent(){}
		EVENT_CLASS_TYPE(ApplicationUpdate)
		EVENT_CLASS_CATEGORY(ApplicationEventCategrory)
	};	
	
	class ApplicationRenderEvent : public Event {
	public:
		ApplicationRenderEvent(){}
		EVENT_CLASS_TYPE(ApplicationRender)
		EVENT_CLASS_CATEGORY(ApplicationEventCategrory)
	};	
	
	class ApplicationTickEvent : public Event {
	public:
		ApplicationTickEvent() {}
		EVENT_CLASS_TYPE(ApplicationTick)
			EVENT_CLASS_CATEGORY(ApplicationEventCategrory)
		};
}