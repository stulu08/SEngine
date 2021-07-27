#pragma once
#include "Stulu/Core/Input.h"

namespace Stulu {
	class WindowsInput : public Input {
	protected:
		virtual bool isKeyDownImpl(unsigned int keycode) override;
		virtual bool isMouseDownImpl(unsigned int button) override;
		virtual std::pair<float, float> getMousePosImpl() override;
		virtual float getMouseXImpl() override;
		virtual float getMouseYImpl() override;
	};
}