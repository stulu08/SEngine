#pragma once
#include "Stulu/Math/Random.h"
#include "Stulu/Renderer/Camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/compatibility.hpp>
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

namespace Stulu {
	struct Quad {
		glm::vec2 pos;
		float width;
		float height;
	};
#define TRANSFORM_UP_DIRECTION glm::vec3(.0f,1.0f,.0f)
#define TRANSFORM_RIGHT_DIRECTION glm::vec3(1.0f,.0f,.0f)
#define TRANSFORM_FOREWARD_DIRECTION glm::vec3(.0f,.0f,-1.0f)

#define PI			3.141592653f
#define PI_2		1.570796326f
#define LOG_2E		1.442695040f
#define M_E			2.718281828f
#define M_RAD2DEG		360.0f / (PI * 2.0f)
	class Math {
	public:
		static const float radiansToDegree(float radians);
		static const float degreeToRadians(float angle);
		static const glm::vec3 QuaternionToEuler(const glm::quat& q);
		static const glm::quat EulerToQuaternion(const glm::vec3& euler);
		static const float clamp(float& v, float min, float max);

		static void setPerlinSeed(uint32_t seed);
		static float simpleNosie(glm::vec2 pos, float offset = 1.0, float scale = 1.0f, glm::vec2 size = glm::vec2(1.0f));
		static float perlinNosie(float x, float y, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
		static float perlinAccumalatedNosie(float x, float y, uint32_t octaves = 1, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
		static float perlinNormalizedNosie(float x, float y, uint32_t octaves = 1, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
	
		static const bool isPosOverQuad(Quad& quad, glm::vec2& pos);
		static const bool isQuadOverQuad(Quad& quad, Quad& quad2);

		static const glm::mat4 createMat4(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale);
		static const glm::mat4 createMat4(const glm::vec3& pos, const glm::quat& rotation, const glm::vec3& scale);
		static const glm::vec3 screenToWorld(const glm::vec2& pos, const glm::mat4& viewProjectionMatrix, glm::vec2& windowSize);

		static const float lookAt2D(const glm::vec3& sourcePoint, const glm::vec3& destPoint);
		static const glm::quat lookAt(const glm::vec3& sourcePoint, const glm::vec3& destPoint);
		static const glm::quat quaternionFromEulerAngle(const glm::vec3& axis, float angle);
		static const bool decomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

	};
}