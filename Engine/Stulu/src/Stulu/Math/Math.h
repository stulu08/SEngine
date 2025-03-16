#pragma once
#include "Stulu/Math/Random.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/glm.hpp>

namespace std {
	inline std::ostream& operator<<(std::ostream& os, const glm::quat& vector) { return os << glm::to_string(vector); }

	inline std::ostream& operator<<(std::ostream& os, const glm::vec4& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::ivec4& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::uvec4& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::dvec4& vector) { return os << glm::to_string(vector); }

	inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::ivec3& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::uvec3& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::dvec3& vector) { return os << glm::to_string(vector); }

	inline std::ostream& operator<<(std::ostream& os, const glm::vec2& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::ivec2& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::uvec2& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::dvec2& vector) { return os << glm::to_string(vector); }

	inline std::ostream& operator<<(std::ostream& os, const glm::mat4& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::mat4x3& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::mat4x2& vector) { return os << glm::to_string(vector); }

	inline std::ostream& operator<<(std::ostream& os, const glm::mat3& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::mat3x4& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::mat3x2& vector) { return os << glm::to_string(vector); }

	inline std::ostream& operator<<(std::ostream& os, const glm::mat2& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::mat2x4& vector) { return os << glm::to_string(vector); }
	inline std::ostream& operator<<(std::ostream& os, const glm::mat2x3& vector) { return os << glm::to_string(vector); }
}


namespace Stulu {
	struct Quad {
		glm::vec2 pos;
		float width;
		float height;
	};
	struct Plane
	{
		glm::vec3 normal = { 0.f, 1.f, 0.f }; // unit vector
		float     distance = 0.f;        // Distance with origin

		Plane() = default;

		Plane(const glm::vec3& p1, const glm::vec3& norm)
			: normal(glm::normalize(norm)),
			distance(glm::dot(normal, p1))
		{}

		float getSignedDistanceToPlan(const glm::vec3& point) const
		{
			return glm::dot(normal, point) - distance;
		}
	};
	struct Frustum
	{
		Plane topFace;
		Plane bottomFace;

		Plane rightFace;
		Plane leftFace;

		Plane farFace;
		Plane nearFace;
	};
	struct AABB {
		glm::vec3 min;
		glm::vec3 max;
	};
	struct PerlinSettings {
		uint32_t octaves = 4;
		float frequency = 1.0f;
		float multiplier = 1.0f;
		glm::vec2 size = glm::vec2(1.0f);
	};
#define TRANSFORM_UP_DIRECTION glm::vec3(.0f,1.0f,.0f)
#define TRANSFORM_RIGHT_DIRECTION glm::vec3(1.0f,.0f,.0f)
#define TRANSFORM_FOREWARD_DIRECTION glm::vec3(.0f,.0f,-1.0f)

	class STULU_API Math {
	public:
		static void setPerlinSeed(uint32_t seed);
		static float simpleNosie(const glm::vec2& pos, const PerlinSettings& settings, bool _0_1_ = true);
		static float simpleNosie(const glm::vec2& pos, float offset = 1.0, float scale = 1.0f, const glm::vec2& size = glm::vec2(1.0f));
		static float perlinNosie(float x, float y, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
		static float perlinAccumalatedNosie(float x, float y, uint32_t octaves = 1, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
		static float perlinNormalizedNosie(float x, float y, uint32_t octaves = 1, float frequenzy = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, bool _0_1_ = true);
	
		static bool isPosOverQuad(const Quad& quad, const glm::vec2& pos);
		static bool isQuadOverQuad(const Quad& quad, const Quad& quad2);
		static bool isAABBOverPos(const AABB& aabb, const glm::vec3& pos);
		static bool isAABBoverAABB(const AABB& aabb1, const AABB& aabb2);

		static glm::vec3 screenToWorld(const glm::vec2& pos, const glm::mat4& viewProjectionMatrix, glm::vec2& windowSize);
		static bool decomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);

		inline static glm::mat4 createMat4(const glm::vec3& pos, const glm::quat& rotation, const glm::vec3& scale) {
			return glm::translate(glm::mat4(1.0f), pos)
				* glm::toMat4(rotation)
				* glm::scale(glm::mat4(1.0f), scale);
		}
		inline static glm::mat4 createMat4(const glm::vec3& pos, const glm::vec3& scale) {
			return glm::translate(glm::mat4(1.0f), pos)
				* glm::scale(glm::mat4(1.0f), scale);
		}

		inline static glm::vec3 QuaternionToEuler(const glm::quat& q) {
			return glm::eulerAngles(q);
		}
		inline static glm::quat EulerToQuaternion(const glm::vec3& euler) {
			return glm::quat(euler);
		}
		inline static glm::vec3 QuaternionToEulerNormalized(const glm::quat& q) {
			glm::vec3 eulerAngles = glm::eulerAngles(q);
			eulerAngles.x = glm::mod(eulerAngles.x + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
			eulerAngles.y = glm::mod(eulerAngles.y + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
			eulerAngles.z = glm::mod(eulerAngles.z + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
			return eulerAngles;
		}
		inline static glm::vec3 NormalizeAngles(const glm::vec3& eulerAngles) {
			const float x = glm::mod(eulerAngles.x + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
			const float y = glm::mod(eulerAngles.y + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
			const float z = glm::mod(eulerAngles.z + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
			return { x, y, z };
		}
		

		inline static float lookAt2D(const glm::vec3& sourcePoint, const glm::vec3& destPoint) {
			glm::vec3 diff = glm::normalize(destPoint - sourcePoint);
			float rot_z = glm::degrees(std::atan2f(diff.y, diff.x));
			return rot_z - 90.0f;
		}
		inline static glm::quat lookAt(const glm::vec3& sourcePoint, const glm::vec3& destPoint, const glm::vec3& up = TRANSFORM_UP_DIRECTION) {
			return glm::quatLookAt(direction(sourcePoint, destPoint), up);
		}

		inline static glm::vec3 direction(const glm::vec3& position, const glm::vec3& destination) {
			return glm::normalize(position - destination);
		}


		template<class T>
		inline static T logBase(T a, T b) {
			return glm::log2(a) / glm::log2(b);
		}
	};


	inline bool Math::isPosOverQuad(const Quad& quad, const glm::vec2& pos) {
		return (
			pos.x >= quad.pos.x && pos.x <= quad.pos.x + quad.width &&
			pos.y >= quad.pos.y && pos.y <= quad.pos.y + quad.height
			);
	}
	inline bool Math::isQuadOverQuad(const Quad& quad1, const Quad& quad2) {
		return (
			quad1.pos.x <= quad2.pos.x + quad2.width && quad1.pos.x + quad1.width >= quad2.pos.x &&
			quad1.pos.y <= quad2.pos.y + quad2.height && quad1.pos.y + quad1.height >= quad2.pos.y
			);
	}
	inline bool Math::isAABBOverPos(const AABB& cube, const glm::vec3& pos) {
		return (
			pos.x >= cube.min.x && pos.x <= cube.max.x &&
			pos.y >= cube.min.y && pos.y <= cube.max.y &&
			pos.z >= cube.min.z && pos.z <= cube.max.z
			);
	}
	inline bool Math::isAABBoverAABB(const AABB& cube1, const AABB& cube2) {
		return (
			cube1.min.x <= cube2.max.x && cube1.max.x >= cube2.min.x &&
			cube1.min.y <= cube2.max.y && cube1.max.y >= cube2.min.y &&
			cube1.min.x <= cube2.max.x && cube1.max.z >= cube2.min.z
			);
	}
	inline glm::vec3 Math::screenToWorld(const glm::vec2& screenPos, const glm::mat4& viewProjectionMatrix, glm::vec2& windowSize) {
		float x = 2.0f * screenPos.x / windowSize.x - 1.0f;
		float y = 2.0f * screenPos.y / windowSize.y - 1.0f;
		glm::vec4 _screenPos = glm::vec4(x, -y, -1.0f, 1.0f);
		glm::mat4 viewProjectionInverse = inverse(viewProjectionMatrix);
		glm::vec4 worldPos = viewProjectionInverse * _screenPos;
		worldPos.w = 1.0f / worldPos.w;
		return glm::vec3(
			worldPos.x * worldPos.w,
			worldPos.y * worldPos.w,
			worldPos.z * worldPos.w);
	}

	inline bool Math::decomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
	{
		// From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3];

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif
		glm::vec3 rot(.0f);
		rot.y = asin(-Row[0][2]);
		if (cos(rot.y) != 0) {
			rot.x = atan2(Row[1][2], Row[2][2]);
			rot.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			rot.x = atan2(-Row[2][0], Row[1][1]);
			rot.z = 0;
		}
		rotation = glm::quat(rot);
		return true;
	}
}