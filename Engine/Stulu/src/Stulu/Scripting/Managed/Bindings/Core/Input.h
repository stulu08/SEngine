#pragma once
#include "../Bindings.h"

namespace StuluBindings {
	class Input {
	public:
		static inline bool getMouse(uint32_t mouse) {
			if (s_enabled)
				return Stulu::Input::isMouseDown(mouse);
			return false;
		}
		static inline bool getMouseDown(uint32_t mouse) {
			if (s_enabled)
				return Stulu::Input::getMouseDown(mouse);
			return false;
		}
		static inline bool getMouseUp(uint32_t mouse) {
			if (s_enabled)
				return Stulu::Input::getMouseUp(mouse);
			return false;
		}
		static inline bool getKey(uint32_t key) {
			if (s_enabled)
				return Stulu::Input::isKeyDown(key);
			return false;
		}
		static inline bool getKeyDown(uint32_t key) {
			if (s_enabled)
				return Stulu::Input::getKeyDown(key);
			return false;
		}
		static inline bool getKeyUp(uint32_t key) {
			if (s_enabled)
				return Stulu::Input::getKeyUp(key);
			return false;
		}
		static inline void setCursorMode(uint32_t mode) {
			if(s_enabled)
				Stulu::Input::setCursorMode((Stulu::Input::CursorMode)mode);
		}
		static inline uint32_t getCursorMode() {
			return (uint32_t)Stulu::Input::getCursorMode();
		}
		static inline void getMouseDelta(struct Vector2* delta) {
			if (s_enabled) {
				delta->x = Stulu::Input::getMouseDelta().x;
				delta->y = Stulu::Input::getMouseDelta().y;
			}
		}
		static inline void getMousePos(struct Vector2* pos) {
			pos->x = Stulu::Input::getMousePos().x;
			pos->y = Stulu::Input::getMousePos().y;
		}

		static inline bool s_enabled = true;
	};
}