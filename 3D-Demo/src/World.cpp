#include "World.h"

World::World() {
	m_defaultShader = Shader::create("assets/shaders/texture.glsl");
	m_defaultShader->bind();
	m_defaultShader->setInt("u_texture", 1);
	m_worldTexture = Texture2D::create("assets/textures/stone.png");
	generate();
}

void World::generate() {
	for (int x = 0; x < 5; x++) {
		for (int z = 0; z < 5; z++) {
			loadedChunks.push_back(Chunk(glm::vec2(x, z), this));
		}
	}

}

void World::update() {

}

void World::draw() {
	m_worldTexture->bind(1);
	for (uint32_t i = 0; i < loadedChunks.size(); i++) {
		loadedChunks[i].draw(m_defaultShader);
	}
}
