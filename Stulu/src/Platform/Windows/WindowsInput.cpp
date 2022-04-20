#include "st_pch.h"
#ifdef ST_PLATFORM_WINDOWS
#include "Stulu/Core/Application.h"
#include "Stulu/Core/Input.h"

#include <GLFW/glfw3.h>

namespace Stulu {
	bool Input::isKeyDown(uint32_t keycode) {
		ST_PROFILING_FUNCTION();
		if (!s_enabled)
			return false;
		auto *window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool Input::isMouseDown(uint32_t button) {
		ST_PROFILING_FUNCTION();
		if (!s_enabled)
			return false;
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	std::pair<float, float> Input::getMousePos() {
		ST_PROFILING_FUNCTION();
		if (!s_enabled)
			return {.0f,.0f};
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
		int32_t m = GLFW_CURSOR_NORMAL;
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
		if (s_enabled)
			glfwSetInputMode(static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow()), GLFW_CURSOR, m);
	}
	Input::CursorMode Input::getCursorMode() {
		ST_PROFILING_FUNCTION();
		uint32_t mode = glfwGetInputMode(static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow()), GLFW_CURSOR);
		switch (mode)
		{
		case GLFW_CURSOR_NORMAL:
			return Stulu::Input::CursorMode::Normal;
		case GLFW_CURSOR_HIDDEN:
			return Stulu::Input::CursorMode::Hidden;
		case GLFW_CURSOR_DISABLED:
			return Stulu::Input::CursorMode::Disabled;
		};
		return Stulu::Input::CursorMode::Disabled;
	}
	static glm::vec2 m_mouseDelta = glm::vec2(0.0f);
	static float m_lastMouseXPos = 0, m_lastMouseYPos = 0;
	glm::vec2 Input::getMouseDelta() {
		ST_PROFILING_FUNCTION();
		if (!s_enabled)
			return glm::vec2(.0f);
		return m_mouseDelta;
	}
	void Input::update() {
		ST_PROFILING_FUNCTION();
		m_mouseDelta = glm::vec2(Input::getMouseX() - m_lastMouseXPos, Input::getMouseY() - m_lastMouseYPos);
		m_lastMouseXPos = Input::getMouseX();
		m_lastMouseYPos = Input::getMouseY();
	}
}
#endif // ST_PLATFORM_WINDOWS