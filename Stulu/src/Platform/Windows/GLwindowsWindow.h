#pragma once

#include "Stulu/Core/Window.h"

#include <GLFW/glfw3.h>
#include "Stulu/Renderer/GraphicsContext.h"

namespace Stulu {
	class GLwindowsWindow : public Window {
	public:
		GLwindowsWindow(const WindowProps& props);
		virtual ~GLwindowsWindow();

		void onUpdate() override;

		inline uint32_t getWidth() const override { return m_data.width; }
		inline uint32_t getHeight() const override { return m_data.height; }

		//attributes
		inline void setEventCallback(const EventCallbackFn& callback) override { m_data.eventCallback = callback; }
		virtual void setWindowIcon(const std::string& path) override;
		void setVSysnc(bool enabled) override;
		bool isVSysnc() const override;

		virtual void* getNativeWindow() const { return m_window; }

	private:
		virtual void init(const WindowProps& props);
		virtual void shutDown();



		GLFWwindow* m_window;
		Scope<GraphicsContext> m_graphicsContext;

		struct WindowData {
			std::string title;
			uint32_t width, height;
			bool VSync;

			EventCallbackFn eventCallback;
		};
		WindowData m_data;
	};
}