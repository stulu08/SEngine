#include "st_pch.h"
#include "WindowsInput.h"
#include "Stulu/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Stulu {

	Input* Input::s_instance = new WindowsInput();

	bool WindowsInput::isKeyDownImpl(unsigned int keycode) {
		auto *window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool WindowsInput::isMouseDownImpl(unsigned int button) {
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	std::pair<float, float> WindowsInput::getMousePosImpl() {
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return {(float)x,(float)y};
	}
	float WindowsInput::getMouseXImpl() {
		auto [x, y] = getMousePos();
		return x;
	}
	float WindowsInput::getMouseYImpl() {
		auto [x, y] = getMousePos();
		return y;
	}
}