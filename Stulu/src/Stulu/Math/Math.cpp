#pragma once
#include "st_pch.h"	
#include "Math.h"
#include "ReputelessPerlinNoise.h"

namespace Stulu {

	siv::PerlinNoise noise = siv::PerlinNoise(0);

	const float Math::radiansToDegree(float radians) {
		return glm::degrees(radians);
	}
	const float Math::degreeToRadians(float angle) {
		return glm::radians(angle);
	}
	const glm::vec3 Math::QuaternionToEuler(const glm::quat& q) {
		return glm::eulerAngles(q) * 180.f / PI;
	}
	const glm::quat Math::EulerToQuaternion(const glm::vec3& euler) {
		return glm::quat(glm::radians(euler));
	}
	const float Math::clamp(float& v, float min, float max) {
		if (v < min)
			v = min;
		else if (v > max)
			v = max;
		return v;
	}
	void Math::setPerlinSeed(uint32_t seed) {
		noise.reseed(seed);
	}
	float Math::simpleNosie(glm::vec2 pos, float offset, float scale, glm::vec2 size) {
		return perlinNosie((pos.x + 0.1f) / size.x * scale + offset, (pos.y + 0.1f) / size.y * scale + offset);
	}
	float Math::perlinNosie(float x, float y, float frequenzy, float sizeX, float sizeY, bool _0_1_) {
		float freqX = sizeX / frequenzy;
		float freqY = sizeY / frequenzy;
		if (_0_1_)
			return (float)noise.noise2D_0_1(x / freqX, y / freqY);
		else
			return (float)noise.noise2D(x / freqX, y / freqY);
	}

	float Math::perlinAccumalatedNosie(float x, float y, uint32_t octaves, float frequenzy, float sizeX, float sizeY, bool _0_1_) {
		
		float freqX = sizeX / frequenzy;
		float freqY = sizeY / frequenzy;
		if(_0_1_)
			return (float)noise.accumulatedOctaveNoise2D_0_1(x / freqX, y / freqY, octaves);
		else
			return (float)noise.accumulatedOctaveNoise2D(x / freqX, y / freqY,octaves);
	}

	float Math::perlinNormalizedNosie(float x, float y, uint32_t octaves, float frequenzy, float sizeX, float sizeY, bool _0_1_) {
		float freqX = sizeX / frequenzy;
		float freqY = sizeY / frequenzy;
		if (_0_1_)
			return (float)noise.normalizedOctaveNoise2D_0_1(x / freqX, y / freqY, octaves);
		else
			return (float)noise.normalizedOctaveNoise2D(x / freqX, y / freqY, octaves);
	}
	const bool Math::isPosOverQuad(Quad& quad, glm::vec2& pos) {
		return (
			pos.x > quad.pos.x && 
			pos.y > quad.pos.y && 
			pos.x < quad.pos.x + quad.width && 
			pos.y < quad.pos.y + quad.height
			);
	}
	const bool Math::isQuadOverQuad(Quad& quad1, Quad& quad2) {
		return (
			quad1.pos.x < quad2.pos.x + quad2.width &&
			quad1.pos.x + quad1.width > quad2.pos.x &&
			quad1.pos.y < quad2.pos.y + quad2.height &&
			quad1.pos.y + quad1.height > quad2.pos.y
			);
	}
	//in degreess
	const glm::mat4 Math::createMat4(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale) {
		if (rotation != glm::vec3(0.0f, 0.0f, 0.0f))
			return glm::translate(glm::mat4(1.0f), pos)
			* glm::toMat4(glm::quat(glm::radians(rotation)))
			* glm::scale(glm::mat4(1.0f), scale);

		return glm::translate(glm::mat4(1.0f), pos)
			* glm::scale(glm::mat4(1.0f), scale);
	}
	const glm::mat4 Math::createMat4(const glm::vec3& pos, const glm::quat& rotation, const glm::vec3& scale)
	{
		return glm::translate(glm::mat4(1.0f), pos)
			* glm::toMat4(rotation)
			* glm::scale(glm::mat4(1.0f), scale);
	}
	const glm::vec3 Math::screenToWorld(const glm::vec2& pos, const glm::mat4& viewProjectionMatrix, glm::vec2& windowSize) {
		double x = 2.0 * pos.x / windowSize.x - 1;
		double y = 2.0 * pos.y / windowSize.y - 1;
		glm::vec4 screenPos = glm::vec4(x, -y, -1.0f, 1.0f);
		glm::mat4 viewProjectionInverse = inverse(viewProjectionMatrix);
		glm::vec4 worldPos = viewProjectionInverse * screenPos;
		return glm::vec3(worldPos);
	}
	//in degrees
	const float Math::lookAt2D(const glm::vec3& sourcePoint, const glm::vec3& destPoint) {
		glm::vec3 diff = glm::normalize(destPoint - sourcePoint);

		float rot_z = std::atan2f(diff.y, diff.x) * M_RAD2DEG;
		return rot_z - 90.0f;
	}
	const glm::quat Math::lookAt(const glm::vec3& sourcePoint, const glm::vec3& destPoint) {
		glm::vec3 forwardVector = glm::normalize(destPoint - sourcePoint);

		float dot = glm::dot(TRANSFORM_FOREWARD_DIRECTION, forwardVector);

		if (std::abs(dot - (-1.0f)) < 0.000001f) {
			return glm::quat(TRANSFORM_UP_DIRECTION.x, TRANSFORM_UP_DIRECTION.y, TRANSFORM_UP_DIRECTION.z, 3.1415926535897932f);
		}
		if (std::abs(dot - (1.0f)) < 0.000001f) {
			return glm::quat(0.0f,0.0f,0.0f,1.0f);
		}

		float rotAngle = (float)std::acos(dot);
		glm::vec3 rotAxis = glm::cross(TRANSFORM_FOREWARD_DIRECTION, forwardVector);
		rotAxis = glm::normalize(rotAxis);
		return quaternionFromEulerAngle(rotAxis, rotAngle);
	}
	const glm::quat Math::quaternionFromEulerAngle(const glm::vec3& axis, float angle) {
		float halfAngle = angle * .5f;
		float s = (float)std::sin(halfAngle);
		glm::quat q;
		q.x = axis.x * s;
		q.y = axis.y * s;
		q.z = axis.z * s;
		q.w = (float)std::cos(halfAngle);
		return q;
	}
	const bool Math::decomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
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

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}

		return true;
	}
}