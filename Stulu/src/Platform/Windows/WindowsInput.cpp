#include "st_pch.h"
#ifdef ST_PLATFORM_WINDOWS
#include "Stulu/Core/Application.h"
#include "Stulu/Core/Input.h"

#include <GLFW/glfw3.h>

namespace Stulu {
	bool Input::isKeyDown(uint32_t keycode) {
		ST_PROFILING_FUNCTION();
		auto *window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
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
		return {(float)x,(float)y};
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
	void Input::setCursorMode(CursorMode mode) {
		ST_PROFILING_FUNCTION();
		int m = GLFW_CURSOR_NORMAL;
		switch (mode)
		{
		case Stulu::Input::CursorMode::Normal:
			m = GLFW_CURSOR_NORMAL;
			break;
		case Stulu::Input::CursorMode::Hidden:
			m = GLFW_CURSOR_HIDDEN;
			break;
		case Stulu::Input::CursorMode::Disabled:
			m = GLFW_CURSOR_DISABLED;
			break;
		default:
			break;
		};
		glfwSetInputMode(static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow()), GLFW_CURSOR, m);
	}
}
#endif // ST_PLATFORM_WINDOWS