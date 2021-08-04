#pragma once

namespace Stulu{
	class Timestep {
	public:
		Timestep(float time)
			: m_time(time) {}

		float getSeconds() const { return m_time; }
		float getMilliseconds() const { return m_time * 1000; }

		operator float() const { return m_time; }
	private:
		float m_time;
	};
	inline std::ostream& operator<<(std::ostream& os, const Timestep& e) {
		return os << e.getSeconds();
	}
}