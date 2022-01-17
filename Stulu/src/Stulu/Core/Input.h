#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Core/Keycodes.h"
#include "Stulu/Core/MouseButtonCodes.h"

namespace Stulu {
	class STULU_API Input {
	public:
		enum class CursorMode{Normal,Hidden,Disabled};
		static bool isKeyDown(uint32_t keycode);
		static bool isMouseDown(uint32_t button);
		static std::pair<float, float> getMousePos();
		static float getMouseX();
		static float getMouseY();
		static void setCursorMode(CursorMode mode);
		static inline glm::vec2 getMouseDelta() {
			return m_mouseDelta;
		}
	private:

		static inline glm::vec2 m_mouseDelta = glm::vec2(0.0f);
		static inline  float m_lastMouseXPos = 0, m_lastMouseYPos = 0;

		static inline void update() {
			m_mouseDelta = glm::vec2(Input::getMouseX() - m_lastMouseXPos, Input::getMouseY() - m_lastMouseYPos);
			m_lastMouseXPos = Input::getMouseX();
			m_lastMouseYPos = Input::getMouseY();
		}

		friend class Application;
	};
}