#pragma once
#include "st_pch.h"

#include "Stulu/Events/Event.h"
#include "Stulu/Renderer/GraphicsContext.h"
#include "Stulu/Math/Math.h"

namespace Stulu{
	enum class WindowAttribute {
		Focused, Iconified, Resizeable, Visible, Decorated,
		Maximized, AutoIconify, Floating, CenterCursor,
		TransparentFramebuffer, Hovered, FocusOnShow
	};
	using EventCallbackFn = std::function<void(Event&)>;
	struct WindowProps {
		std::string title;
		uint32_t width, height;
		bool VSync;
		EventCallbackFn eventCallback;

		WindowProps(const std::string& title = "Stulu Engine", uint32_t width = 1280, uint32_t height = 720, bool VSync = false)
			: title(title), width(width),height(height), VSync(VSync), eventCallback() {}
	};
	class STULU_API Window{
	public:

		virtual ~Window() = default;

		virtual void onUpdate() = 0;
		virtual void hide() = 0;
		virtual void show() = 0;
		virtual void setAttribute(const WindowAttribute, int32_t value) = 0;
		virtual int getAttribute(const WindowAttribute) = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual glm::vec2 getWindowPos() const = 0;

		//attributes
		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setWindowIcon(const std::string& path) = 0;
		virtual void setWindowTitle(const std::string& title) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;
		virtual Scope<GraphicsContext>& getContext() = 0;
		static Scope<Window> create(WindowProps& props = WindowProps());
	};
}