#pragma once
#include <glm/glm.hpp>
#include <vector>

class BlockData {
public:
	static glm::vec3 verts[8];
	static glm::vec2 uvs[4];
	static uint32_t tris[6][4];
	static glm::vec3 normals[6];
	static glm::vec3 faceChecks[6];
};
class ChunkData {
public:
	static const int width = 16;
	static const int depth = 16;
	static const int height = 16;
};
class DefaultBiome {
public:
	static const int baseHeight = 51;
	static const int height = 51;
	static const int scale = 51;
};
class Settings {
public:

};