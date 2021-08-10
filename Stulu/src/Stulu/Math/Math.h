#pragma once
#include "Stulu/Math/Random.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>



namespace Stulu {

#define PI		3.141592653f
#define PI_2	1.570796326f
#define LOG_2E	1.442695040f
#define E		2.718281828f
	class Math {
	public:
		static float radiansToDegree(float radians);
		static float degreeToRadians(float angle);
		static glm::vec3 QuaternionToEuler(glm::quat q);
		static glm::quat EulerToQuaternion(glm::vec3 euler);
		static float clamp(float& v, float min, float max);

		static void setPerlinSeed(uint32_t seed);
		static float perlinNosie(float x, float y, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
		static float perlinAccumalatedNosie(float x, float y, uint32_t octaves = 1, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
		static float perlinNormalizedNosie(float x, float y, uint32_t octaves = 1, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
	};
}