#include "st_pch.h"
#ifdef ST_PLATFORM_LINUX
#include "Stulu/Core/Application.h"
#include "Stulu/Input/Input.h"

#include <GLFW/glfw3.h>

namespace Stulu {
	static bool s_enabled = true;

	void Input::setEnabled(bool value) {
		s_enabled = value;
	}
	bool Input::getEnabled() {
		return s_enabled;
	}

	bool Input::isKeyDown(uint32_t keycode) {
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool Input::isMouseDown(uint32_t button) {
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	std::pair<float, float> Input::getMousePos() {
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return { (float)x,(float)y };
	}
	float Input::getMouseX() {
		auto [x, y] = getMousePos();
		return x;
	}
	float Input::getMouseY() {
		auto [x, y] = getMousePos();
		return y;
	}
}
#endif // ST_PLATFORM_LINUX