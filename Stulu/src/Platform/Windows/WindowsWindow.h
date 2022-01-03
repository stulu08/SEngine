#pragma once

#include "Stulu/Core/Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Stulu {
	class STULU_API WindowsWindow : public Window {
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void onUpdate() override;
		void hide() override;
		void show() override;

		void setAttribute(const WindowAttribute attribute, int32_t value) override;
		int getAttribute(const WindowAttribute attribute) override;

		inline uint32_t getWidth() const override { return m_runtimeData.width; }
		inline uint32_t getHeight() const override { return m_runtimeData.height; }

		//attributes
		inline void setEventCallback(const EventCallbackFn& callback) override { m_runtimeData.eventCallback = callback; }
		virtual void setWindowIcon(const std::string& path) override;
		virtual void setWindowTitle(const std::string& title) override;
		void setVSync(bool enabled) override;
		bool isVSync() const override;

		virtual void* getNativeWindow() const { return m_window; }
		virtual Scope<GraphicsContext>& getContext() override { return m_graphicsContext; }
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
		WindowData m_runtimeData;
	};
}