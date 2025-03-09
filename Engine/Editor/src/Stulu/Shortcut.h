#include <Stulu/Core/Input.h>

namespace Editor {
	class Shortcut {
	public:
		Shortcut(std::function<bool()> function, Stulu::Keyboard::KeyCode code, bool shift = false, bool ctrl = false, bool alt = false)
			: m_function(function), m_code(code), m_shift(shift), m_ctrl(ctrl), m_alt(alt){}

		inline Stulu::Keyboard::KeyCode GetKeyCode() const { return m_code; };

		inline bool UseShift() const { return m_shift; };
		inline bool UseControl() const { return m_ctrl; };
		inline bool UseAlt() const { return m_alt; };
		
		inline virtual bool OnPress() const {
			if (m_function)
				return m_function();
			return false;
		};

	private:
		bool m_shift, m_ctrl, m_alt;
		Stulu::Keyboard::KeyCode m_code;
		std::function<bool()> m_function;
	};
}