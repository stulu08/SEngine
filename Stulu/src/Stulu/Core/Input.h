#pragma once
#include "Stulu/Math/Math.h"
#include "Stulu/Core/Core.h"
#include "Stulu/Core/Keycodes.h"
#include "Stulu/Core/MouseButtonCodes.h"

namespace Stulu {
	class STULU_API Input {
	public:
		enum class CursorMode{Normal=0,Hidden=1,Disabled=2};
		static bool isKeyDown(uint32_t keycode);
		static bool isMouseDown(uint32_t button);
		static std::pair<float, float> getMousePos();
		static float getMouseX();
		static float getMouseY();
		static void setCursorMode(CursorMode mode);
		static CursorMode getCursorMode();
		static glm::vec2 getMouseDelta();
	private:
		static void update();
		static inline bool s_enabled;

		friend class EditorLayer;
		friend class Application;
	};
}