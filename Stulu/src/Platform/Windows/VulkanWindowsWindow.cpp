#include "st_pch.h"
#include "VulkanWindowsWindow.h"

#include <stb_image.h>

#include <Stulu/Events/ApplicationEvent.h>
#include <Stulu/Events/KeyEvent.h>
#include <Stulu/Events/MouseEvent.h>

#include "Platform/Vulkan/VulkanContext.h"

namespace Stulu {
	static bool s_glfwInitilized = false;

	static void glfwErrorCallback(int32_t error, const char* msg) {
		CORE_ERROR("GLFW error {0}: {1}" , error , msg)
	}

	VulkanWindowsWindow::VulkanWindowsWindow(const WindowProps& props) {
		ST_PROFILING_FUNCTION();
		init(props);
	}
	VulkanWindowsWindow::~VulkanWindowsWindow() { shutDown(); }


	void VulkanWindowsWindow::init(const WindowProps& props) {
		ST_PROFILING_FUNCTION();
		m_data.title = props.title;
		m_data.width = props.width;
		m_data.height = props.height;


		CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (!s_glfwInitilized) {

			int32_t succes = glfwInit();
			CORE_ASSERT(succes, "Could not initialize GLFW!");
			glfwSetErrorCallback(glfwErrorCallback);
			s_glfwInitilized = true;
			if (!glfwVulkanSupported()) {
				CORE_ASSERT(false, "Vulkan is not supported");
			}
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr); 

		m_graphicsContext = createScope<VulkanContext>(m_window);

		m_graphicsContext->init();

		glfwSetWindowUserPointer(m_window, &m_data);
		

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;
			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});
		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.eventCallback(event);
		});
		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancodem, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
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
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.eventCallback(event);

		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
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
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrollEvent event((float)x, (float)y);
			data.eventCallback(event);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMoveEvent event((float)x, (float)y);
			data.eventCallback(event);
		});
	}
	void VulkanWindowsWindow::shutDown() {
		ST_PROFILING_FUNCTION();
		glfwDestroyWindow(m_window);
	}
	void VulkanWindowsWindow::onUpdate() {
		ST_PROFILING_FUNCTION();
		glfwPollEvents();

		m_graphicsContext->swapBuffers();
	}

	void VulkanWindowsWindow::setWindowIcon(const std::string& path) {
		ST_PROFILING_FUNCTION();
		int32_t width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		stbi_uc* textureData = stbi_load(path.c_str(), &width, &height, &channels, 0);
		CORE_ASSERT(textureData, "Icon failed to load: {0}", path);
		GLFWimage images[1];
		images[0] = GLFWimage();
		images[0].height = height;
		images[0].width = width;
		images[0].pixels = textureData;

		glfwSetWindowIcon(m_window, 1, images);
	}
	void VulkanWindowsWindow::setWindowTitle(const std::string& title) {
		ST_PROFILING_FUNCTION();
		glfwSetWindowTitle(m_window, title.c_str());
	}
	void VulkanWindowsWindow::setVSysnc(bool enabled) {
		ST_PROFILING_FUNCTION();
		
		m_data.VSync = enabled;
	}

	bool VulkanWindowsWindow::isVSysnc() const { return m_data.VSync; }
}