#pragma once
#include "Stulu/Input.h"

namespace Stulu {
	class WindowsInput : public Input {
	protected:
		virtual bool isKeyDownImpl(int keycode) override;
		virtual bool isMouseDownImpl(int button) override;
		virtual std::pair<float, float> getMousePosImpl() override;
		virtual float getMouseXImpl() override;
		virtual float getMouseYImpl() override;
	};
}