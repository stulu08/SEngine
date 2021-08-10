#pragma once
#include "st_pch.h"	
#include "Math.h"
#include "ReputelessPerlinNoise.h"

namespace Stulu {

#define PI		3.141592653f
#define PI_2	1.570796326f
#define LOG_2E	1.442695040f
#define E		2.718281828f

	siv::PerlinNoise noise = siv::PerlinNoise(0);

	float Math::radiansToDegree(float radians) {
		return glm::degrees(radians);
	}
	float Math::degreeToRadians(float angle) {
		return glm::radians(angle);
	}
	glm::vec3 Math::QuaternionToEuler(glm::quat q) {
		return glm::eulerAngles(q) * 180.f / PI;
	}
	glm::quat Math::EulerToQuaternion(glm::vec3 euler) {
		return glm::quat(glm::radians(euler));
	}
	float Math::clamp(float& v, float min, float max) {
		if (v < min)
			v = min;
		else if (v > max)
			v = max;
		return v;
	}
	void Math::setPerlinSeed(uint32_t seed) {
		noise.reseed(seed);
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
}