#pragma once

#include "Stulu/Core/Window.h"

#include <GLFW/glfw3.h>
#include "Stulu/Renderer/GraphicsContext.h"

namespace Stulu {
	class WindowsWindow : public Window {
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void onUpdate() override;

		inline unsigned int getWidth() const override { return m_data.width; }
		inline unsigned int getHeight() const override { return m_data.height; }

		//attributes
		inline void setEventCallback(const EventCallbackFn& callback) override { m_data.eventCallback = callback; }
		void setVSysnc(bool enabled) override;
		bool isVSysnc() const override;

		virtual void* getNativeWindow() const { return m_window; }

	private:
		virtual void init(const WindowProps& props);
		virtual void shutDown();



		GLFWwindow* m_window;
		GraphicsContext* m_graphicsContext;

		struct WindowData {
			std::string title;
			unsigned int width, height;
			bool VSync;

			EventCallbackFn eventCallback;
		};
		WindowData m_data;
	};
}