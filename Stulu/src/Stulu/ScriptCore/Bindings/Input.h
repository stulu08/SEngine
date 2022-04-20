#pragma once
#include "Stulu/Core/Input.h"
#include "Stulu/ScriptCore/Bindings/Types.h"
#include <mono/metadata/object.h>

namespace StuluBindings {
	class Input {
	public:
		static inline bool mouseDown(uint32_t mouse) {
			if (s_enabled)
				return Stulu::Input::isMouseDown(mouse);
			return false;
		}
		static inline bool keyDown(uint32_t key) {
			if (s_enabled)
				return Stulu::Input::isKeyDown(key);
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
			pos->x = Stulu::Input::getMousePos().first;
			pos->y = Stulu::Input::getMousePos().second;
		}

		static inline bool s_enabled = true;
	};
}