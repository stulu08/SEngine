#pragma once
#include "Stulu/Core/Input.h"
#include <mono/metadata/object.h>

namespace StuluBindings {
	class Input {
	public:
		static inline bool mouseDown(uint32_t mouse) {
			return Stulu::Input::isMouseDown(mouse);
		}
		static inline bool keyDown(uint32_t key) {
			return Stulu::Input::isKeyDown(key);
		}
		static inline void setCursorMode(uint32_t mode) {
			Stulu::Input::setCursorMode((Stulu::Input::CursorMode)mode);
		}
	};
}