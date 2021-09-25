#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Core/Keycodes.h"
#include "Stulu/Core/MouseButtonCodes.h"

namespace Stulu {
	class Input {
	public:
		static bool isKeyDown(uint32_t keycode);
		static bool isMouseDown(uint32_t button);
		static std::pair<float, float> getMousePos();
		static float getMouseX();
		static float getMouseY();
	};
}