#pragma once

#include "Stulu/Window.h"

#include <GLFW/glfw3.h>

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

	private:
		virtual void init(const WindowProps& props);
		virtual void shutDown();
	private:
		GLFWwindow* m_window;

		struct WindowData {
			std::string title;
			unsigned int width, height;
			bool VSync;

			EventCallbackFn eventCallback;
		};
		WindowData m_data;
	};
}