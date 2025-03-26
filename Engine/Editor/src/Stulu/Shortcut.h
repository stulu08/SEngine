#include <Stulu/Core/Input.h>
#include <magic_enum/magic_enum.hpp>

namespace Editor {
	class Shortcut {
	public:
		Shortcut(const std::string& name, std::function<bool()> function, Stulu::Keyboard::KeyCode code, bool shift = false, bool ctrl = false, bool alt = false)
			: m_name(name), m_function(function), m_code(code), m_shift(shift), m_ctrl(ctrl), m_alt(alt){}

		inline Stulu::Keyboard::KeyCode GetKeyCode() const { return m_code; };

		inline bool UseShift() const { return m_shift; };
		inline bool UseControl() const { return m_ctrl; };
		inline bool UseAlt() const { return m_alt; };
		inline const char* GetName() const { return m_name.c_str(); }

		inline std::string BuildKeyString() const {
			std::stringstream stream;
			if (UseControl())
				stream << "Ctrl+";
			if (UseShift())
				stream << "Shift+";
			if (UseAlt())
				stream << "Alt+";

			stream << magic_enum::enum_name((Stulu::Keyboard::Enum)GetKeyCode());

			return stream.str();
		}

		inline virtual bool OnPress() const {
			if (m_function)
				return m_function();
			return false;
		};

	private:
		std::string m_name;
		bool m_shift, m_ctrl, m_alt;
		Stulu::Keyboard::KeyCode m_code;
		std::function<bool()> m_function;
	};

	// have shortcuts with mods execute at first, since without mods are likely to always succed thus blocking one behind
	inline bool operator>(const Shortcut& left, const Shortcut& right) {
		uint8_t modSumLeft = 0, modSumRight = 0;

		if (left.UseAlt())
			modSumLeft++;
		if (left.UseShift())
			modSumLeft++;
		if (left.UseControl())
			modSumLeft++;

		if (right.UseAlt())
			modSumRight++;
		if (right.UseShift())
			modSumRight++;
		if (right.UseControl())
			modSumRight++;

		return modSumLeft > modSumRight;
	}
}