#pragma once
#include "st_pch.h"

#include "Stulu/Events/Event.h"

namespace Stulu{
	
	struct WindowProps {
		std::string title;
		uint32_t width;
		uint32_t height;

		WindowProps(const std::string& _title = "Stulu Engine", uint32_t _width = 1280, uint32_t _height = 720)
			: title(_title), width(_width),height(_height){}
	};

	class Window{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void onUpdate() = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		//attributes
		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setWindowIcon(const std::string& path) = 0;
		virtual void setVSysnc(bool enabled) = 0;
		virtual bool isVSysnc() const = 0;

		virtual void* getNativeWindow() const = 0;

		static Scope<Window> create(const WindowProps& props = WindowProps());
	};
}