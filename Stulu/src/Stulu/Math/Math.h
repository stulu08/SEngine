#pragma once
#include "Stulu/Math/Random.h"
#include "Stulu/Renderer/Camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

namespace Stulu {
	struct Quad {
		glm::vec2 pos;
		float width;
		float height;
	};

#define PI		3.141592653f
#define PI_2	1.570796326f
#define LOG_2E	1.442695040f
#define E		2.718281828f
	class Math {
	public:
		static float radiansToDegree(float radians);
		static float degreeToRadians(float angle);
		const static glm::vec3 QuaternionToEuler(const glm::quat& q);
		const static glm::quat EulerToQuaternion(const glm::vec3& euler);
		static float clamp(float& v, float min, float max);

		static void setPerlinSeed(uint32_t seed);
		static float perlinNosie(float x, float y, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
		static float perlinAccumalatedNosie(float x, float y, uint32_t octaves = 1, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
		static float perlinNormalizedNosie(float x, float y, uint32_t octaves = 1, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
	
		static bool isPosOverQuad(Quad& quad, glm::vec2& pos);
		static bool isQuadOverQuad(Quad& quad, Quad& quad2);

		static const glm::mat4 createMat4(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale);
		static const glm::vec3 screenToWorld(const glm::vec2& pos, const Camera& cam, glm::vec2& windowSize);
	};
}