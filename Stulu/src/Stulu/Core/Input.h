#pragma once
#include "Stulu/Core/Core.h"

namespace Stulu {
	class Input {
	protected:
		Input() = default;
	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;
		inline static bool isKeyDown(unsigned int keycode) { return s_instance->isKeyDownImpl(keycode); }

		inline static bool isMouseDown(unsigned int button) { return s_instance->isMouseDownImpl(button); }

		inline static std::pair<float, float> getMousePos() { return s_instance->getMousePosImpl(); }

		inline static float getMouseX() { return s_instance->getMouseXImpl(); }
		inline static float getMouseY() { return s_instance->getMouseYImpl(); }
	protected:
		virtual bool isKeyDownImpl(unsigned int keycode) = 0;
		virtual bool isMouseDownImpl(unsigned int button) = 0;

		virtual std::pair<float, float> getMousePosImpl() = 0;

		virtual float getMouseXImpl() = 0;
		virtual float getMouseYImpl() = 0;
	private:
		static Scope<Input> s_instance;
	};
}