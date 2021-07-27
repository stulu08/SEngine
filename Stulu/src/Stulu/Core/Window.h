#pragma once
#include "st_pch.h"

#include "Stulu/Events/Event.h"

namespace Stulu{
	
	struct WindowProps {
		std::string title;
		unsigned int width;
		unsigned int height;

		WindowProps(const std::string& _title = "Stulu Engine", unsigned int _width = 1280, unsigned int _height = 720)
			: title(_title), width(_width),height(_height){}
	};

	class Window{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {};

		virtual void onUpdate() = 0;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;

		//attributes
		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSysnc(bool enabled) = 0;
		virtual bool isVSysnc() const = 0;

		virtual void* getNativeWindow() const = 0;

		static Window* create(const WindowProps& props = WindowProps());
	};
}