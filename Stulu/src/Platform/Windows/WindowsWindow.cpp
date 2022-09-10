#include "st_pch.h"
#include "WindowsWindow.h"

#include <stb_image.h>

#include <Stulu/Events/ApplicationEvent.h>
#include <Stulu/Events/KeyEvent.h>
#include <Stulu/Events/MouseEvent.h>
#include <Stulu/Renderer/Renderer.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Stulu {
	static bool s_glfwInitilized = false;

	static void glfwErrorCallback(int32_t error, const char* msg) {
		CORE_ERROR("GLFW error {0}: {1}" , error , msg)
	}

	WindowsWindow::WindowsWindow(WindowProps& props) {
		ST_PROFILING_FUNCTION();
		init(props);
	}
	WindowsWindow::~WindowsWindow() { shutDown(); }


	void WindowsWindow::init(WindowProps& props) {
		ST_PROFILING_FUNCTION();
		m_runtimeData = &props;
		CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (!s_glfwInitilized) {

			int32_t succes = glfwInit();
			CORE_ASSERT(succes, "Could not initialize GLFW!");
			glfwSetErrorCallback(glfwErrorCallback);
			s_glfwInitilized = true;
		}
		if (Renderer::getRendererAPI() == RenderAPI::API::OpenGL) {
#ifndef ST_DIST
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		}
		else if (Renderer::getRendererAPI() == RenderAPI::API::GLES) {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
		}
		else if (Renderer::getRendererAPI() == RenderAPI::API::Vulkan){
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		}
		m_graphicsContext = GraphicsContext::create();
		m_window = glfwCreateWindow((int)props.width, (int)props.height, props.title.c_str(), nullptr, nullptr);
		m_graphicsContext->init(this);

		glfwSetWindowUserPointer(m_window, m_runtimeData);
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;
			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});
		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
			WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.eventCallback(event);
		});
		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancodem, int action, int mods) {
			WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
			
			switch (action) {
				case GLFW_PRESS: {
					KeyDownEvent event(key, 0);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					KeyUpEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT: {
					KeyDownEvent event(key, 1);
					data.eventCallback(event);
					break;
				}
			}

		});
		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode) {
			WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.eventCallback(event);

		});
		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
			switch (action) {
				case GLFW_PRESS: {
					MouseButtonDownEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					MouseButtonUpEvent event(button);
					data.eventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x, double y) {
			WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
			MouseScrollEvent event((float)x, (float)y);
			data.eventCallback(event);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y) {
			WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
			MouseMoveEvent event((float)x, (float)y);
			data.eventCallback(event);
		});
	}
	void WindowsWindow::shutDown() {
		ST_PROFILING_FUNCTION();
		glfwDestroyWindow(m_window);
	}
	void WindowsWindow::onUpdate() {
		ST_PROFILING_FUNCTION();
		glfwPollEvents();

		m_graphicsContext->swapBuffers();
	}

	glm::vec2 WindowsWindow::getWindowPos() const {
		ST_PROFILING_FUNCTION();
		int x, y;
		glfwGetWindowPos(m_window, &x, &y);
		return glm::vec2(x,y);
	}

	void WindowsWindow::setWindowIcon(const std::string& path) {
		ST_PROFILING_FUNCTION();
		int32_t width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		stbi_uc* textureData = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!textureData) {
			CORE_ERROR("Icon failed to load: {0}", path);
			return;
		}
		GLFWimage images[1];
		images[0] = GLFWimage();
		images[0].height = height;
		images[0].width = width;
		images[0].pixels = textureData;

		glfwSetWindowIcon(m_window, 1, images);
	}
	void WindowsWindow::setWindowTitle(const std::string& title) {
		ST_PROFILING_FUNCTION();
		glfwSetWindowTitle(m_window, title.c_str());
		m_runtimeData->title = title;
	}
	void WindowsWindow::setVSync(bool enabled) {
		ST_PROFILING_FUNCTION();
		m_graphicsContext->setVSync(enabled);
		m_runtimeData->VSync = enabled;
	}
	void WindowsWindow::hide() {
		ST_PROFILING_FUNCTION();
		glfwHideWindow(m_window);
	}
	void WindowsWindow::show() {
		ST_PROFILING_FUNCTION();
		glfwShowWindow(m_window);
	}
	void WindowsWindow::setAttribute(const WindowAttribute attribute, int32_t value) {
		ST_PROFILING_FUNCTION();
		glfwSetWindowAttrib(m_window, 0x00020001 + (int)attribute, value);
	}
	int WindowsWindow::getAttribute(const WindowAttribute attribute) {
		ST_PROFILING_FUNCTION();
		return glfwGetWindowAttrib(m_window, 0x00020001 + (int)attribute);
	}
	bool WindowsWindow::isVSync() const { return m_runtimeData->VSync; }
}