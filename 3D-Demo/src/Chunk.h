#pragma once
#include <Stulu.h>
#include "Constants.h"

using namespace Stulu;

class World;

class Chunk {
public:
	Chunk(glm::vec2 pos, World* world);
	void generate();
	void draw(const Ref<Shader>& shader);
	bool checkBlock(glm::vec3 pos);


	static inline glm::vec2 toChunkCoord(glm::vec3 pos) {
		return glm::vec2(std::floorf(pos.x / ChunkData::width), std::floorf(pos.y / ChunkData::depth));
	}
	static inline glm::vec3 toWorldPos(glm::vec2 chunkCoord) {
		return glm::vec3(chunkCoord.x * ChunkData::width, .0f, chunkCoord.y * ChunkData::depth);
	}
private:
	void populate();
	void createMesh();
	void genBlock(glm::vec3& pos);
	void reCalcMesh();

	glm::vec3 worldPos;
	World* world;
	Mesh mesh;
	int vertexIndex = 0;
	uint8_t blockMap[ChunkData::width][ChunkData::height][ChunkData::depth];
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

