#pragma once
#include <glm/glm.hpp>

namespace Stulu {
	class Random {
	public:
		static int getInt(int min, int max);
		static float getFloat(float min = 0.0f, float max = 1.0);
	};
}