#pragma once
#include "Chunk.h"

using namespace Stulu;

struct Block {
	glm::vec2 texturePixelPos;
	glm::vec2 texturePixelSize;
};

class World {
public:
	World();
	void generate();
	void update();
	void draw();
private:
	std::vector<Chunk> loadedChunks = std::vector<Chunk>();
	Ref<Texture2D> m_worldTexture;
	Ref<Shader> m_defaultShader;
};