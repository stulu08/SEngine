#include "st_pch.h"
#ifdef ST_PLATFORM_LINUX
#include "Stulu/Core/Application.h"
#include "Stulu/Core/Input.h"

#include <GLFW/glfw3.h>

namespace Stulu {
	bool Input::isKeyDown(uint32_t keycode) {
		ST_PROFILING_FUNCTION();
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool Input::isMouseDown(uint32_t button) {
		ST_PROFILING_FUNCTION();
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	std::pair<float, float> Input::getMousePos() {
		ST_PROFILING_FUNCTION();
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return { (float)x,(float)y };
	}
	float Input::getMouseX() {
		ST_PROFILING_FUNCTION();
		auto [x, y] = getMousePos();
		return x;
	}
	float Input::getMouseY() {
		ST_PROFILING_FUNCTION();
		auto [x, y] = getMousePos();
		return y;
	}
}
#endif // ST_PLATFORM_LINUX