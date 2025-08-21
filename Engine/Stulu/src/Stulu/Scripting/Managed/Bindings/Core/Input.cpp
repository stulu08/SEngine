#include "st_pch.h"
#include "Input.h"
#include "Stulu/Input/Input.h"

namespace StuluBindings {
	bool Input::s_enabled = true;

	bool Input::getMouse(uint32_t mouse) {
		if (s_enabled)
			return Stulu::Input::isMouseDown(mouse);
		return false;
	}
	bool Input::getMouseDown(uint32_t mouse) {
		if (s_enabled)
			return Stulu::Input::getMouseDown(mouse);
		return false;
	}
	bool Input::getMouseUp(uint32_t mouse) {
		if (s_enabled)
			return Stulu::Input::getMouseUp(mouse);
		return false;
	}
	bool Input::getKey(uint32_t key) {
		if (s_enabled)
			return Stulu::Input::isKeyDown(key);
		return false;
	}
	bool Input::getKeyDown(uint32_t key) {
		if (s_enabled)
			return Stulu::Input::getKeyDown(key);
		return false;
	}
	bool Input::getKeyUp(uint32_t key) {
		if (s_enabled)
			return Stulu::Input::getKeyUp(key);
		return false;
	}
	void Input::setCursorMode(uint32_t mode) {
		if (s_enabled)
			Stulu::Input::setCursorMode((Stulu::Input::CursorMode)mode);
	}
	uint32_t Input::getCursorMode() {
		return (uint32_t)Stulu::Input::getCursorMode();
	}
	void Input::getMouseDelta(struct Vector2* delta) {
		if (s_enabled) {
			delta->x = Stulu::Input::getMouseDelta().x;
			delta->y = Stulu::Input::getMouseDelta().y;
		}
	}
	void Input::getMousePos(struct Vector2* pos) {
		pos->x = Stulu::Input::getMousePos().x;
		pos->y = Stulu::Input::getMousePos().y;
	}

	void Input::SetEnabled(bool value) {
		s_enabled = value;
	}
}