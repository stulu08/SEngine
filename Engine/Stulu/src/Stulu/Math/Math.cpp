#pragma once
#include "st_pch.h"	
#include "Math.h"
#include "ReputelessPerlinNoise.h"

namespace Stulu {

	siv::PerlinNoise noise = siv::PerlinNoise(0);

	void Math::setPerlinSeed(uint32_t seed) {
		noise.reseed(seed);
	}
	float Math::simpleNosie(const glm::vec2& pos, const PerlinSettings& con, bool _0_1_) {
		float freqX = con.size.x / con.frequency;
		float freqY = con.size.y / con.frequency;
		if (_0_1_)
			return (float)noise.accumulatedOctaveNoise2D_0_1(pos.x / freqX, pos.y / freqY, con.octaves) * con.multiplier;
		else
			return (float)noise.accumulatedOctaveNoise2D(pos.x / freqX, pos.y / freqY, con.octaves) * con.multiplier;
	}
	float Math::simpleNosie(const glm::vec2& pos, float offset, float scale, const glm::vec2& size) {
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
}