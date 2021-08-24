#pragma once
#include <glm/glm.hpp>

namespace Stulu {
	class Random {
	public:
		static int getInt(int min, int max);
		static float getFloat(float min = 0.0f, float max = 1.0);
		static glm::vec2 getVector2(glm::vec2 min, glm::vec2 max);
		static glm::vec3 getVector3(glm::vec3 min, glm::vec3 max);
	};
}