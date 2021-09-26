#pragma once
#include "Stulu/Math/Math.h"
namespace Stulu{
	class Timestep {
	public:
		Timestep(float time)
			: m_time(time) {}

		float getSeconds() const { return m_time; }
		float getMilliseconds() const { return m_time * 1000; }

		operator float() const { return m_time; }

		glm::vec3 operator* (const glm::vec3& v) const { return v * m_time; }
		glm::vec2 operator* (const glm::vec2& v) const { return v * m_time; }
	private:
		float m_time;
	};
	inline std::ostream& operator<<(std::ostream& os, const Timestep& e) {
		return os << e.getSeconds();
	}
	inline glm::vec4& operator* (const glm::vec4& v, const Timestep& ts) {
		return v * ts.getSeconds();
	}
	inline glm::vec3& operator* (const glm::vec3& v, const Timestep& ts){
		return v * ts.getSeconds();
	}
	inline glm::vec2& operator* (const glm::vec2& v, const Timestep& ts){
		return v * ts.getSeconds();
	}
}