#pragma once
#include "../Bindings.h"

namespace StuluBindings {
	class STULU_API Input {
	public:
		static bool getMouse(uint32_t mouse);
		static bool getMouseDown(uint32_t mouse);
		static bool getMouseUp(uint32_t mouse);
		static bool getKey(uint32_t key);
		static bool getKeyDown(uint32_t key);
		static bool getKeyUp(uint32_t key);
		static void setCursorMode(uint32_t mode);
		static uint32_t getCursorMode();
		static void getMouseDelta(struct Vector2* delta);
		static void getMousePos(struct Vector2* pos);

		static void SetEnabled(bool value);

	private:
		static bool s_enabled;
	};
}