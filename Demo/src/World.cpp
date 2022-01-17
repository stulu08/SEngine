#include "World.h"
#include "Chunk.h"
#include "GameLayer.h"

namespace Demo {
	void World::onStart() {
		for (int i = 0; i < WORLD_SIZE_X; i++) {
			for (int j = 0; j < WORLD_SIZE_X; j++) {
				loadedChunks[glm::vec2(i, j)] = createChunk();
			}
		}
		textureAtals->bind(3);
	}
	void World::onUpdate() {
		for (auto& v : loadedChunks) {
			GameObject chunkObject = v.second;
			if (chunkObject && chunkObject.hasComponent<NativeBehaviourComponent>()) {
				auto& nbC = chunkObject.getComponent<NativeBehaviourComponent>();
				if (!nbC.instance) {
					continue;
				}
				Chunk* chunk = static_cast<Chunk*>(nbC.instance);
				if (!chunk->generated) {
					chunk->world = this;
					chunk->generate(v.first);
				}
				else {
					if(chunk->enabled)
						chunk->enabled = true;
				}
			}
		}
	}
	GameObject World::createChunk() {
		GameObject go = getScene()->createGameObject("Chunk");
		go.addComponent<NativeBehaviourComponent>().bind<Chunk>();
		getComponent<TransformComponent>().addChild(go);
		return go;
	}
	Chunk* World::getChunk(const ChunkCoords& coords) {
		GameObject chunkObject = loadedChunks[coords];
		Chunk* chunk = nullptr;
		if (chunkObject && chunkObject.hasComponent<NativeBehaviourComponent>()) {
			auto& nbC = chunkObject.getComponent<NativeBehaviourComponent>();
			if (!nbC.instance) {
				return nullptr;
			}
			chunk = static_cast<Chunk*>(nbC.instance);
		}
		return chunk;
	}
	void World::loadBlocks() {
		textureAtals = Texture2D::create("assets/atlas.png");
		blocks[BlockType::BedRock] = Block(BlockType::BedRock, SubTexture2D::fromCoords(glm::vec2(6,1),glm::vec2(16.0f), textureAtals), false);
		blocks[BlockType::Air] = Block(BlockType::Air, SubTexture2D::fromCoords(glm::vec2(0,0),glm::vec2(16.0f), textureAtals), true);
		blocks[BlockType::Stone] = Block(BlockType::Stone, SubTexture2D::fromCoords(glm::vec2(5,2),glm::vec2(16.0f), textureAtals), false);
		blocks[BlockType::Grass] = Block(BlockType::Grass, SubTexture2D::fromCoords(glm::vec2(1,0),glm::vec2(16.0f), textureAtals), SubTexture2D::fromCoords(glm::vec2(0,1), glm::vec2(16.0f), textureAtals), SubTexture2D::fromCoords(glm::vec2(8, 1), glm::vec2(16.0f), textureAtals), false);
		blocks[BlockType::Dirt] = Block(BlockType::Dirt, SubTexture2D::fromCoords(glm::vec2(8, 1), glm::vec2(16.0f), textureAtals), false);
		blocks[BlockType::DiamondBlock] = Block(BlockType::DiamondBlock, SubTexture2D::fromCoords(glm::vec2(13, 9), glm::vec2(16.0f), textureAtals), false, Block::Material{ .9f,.8f,.4f });
	}
	bool World::isTransparent(BlockType type) {
		return blocks[type].transparent;
	}
	BlockType World::getBlockFromPerlin(const glm::vec3& pos) {
		if (pos.y == 0)
			return BlockType::BedRock;
		float noise = glm::floor(Math::perlinAccumalatedNosie((float)pos.x, (float)pos.z, 8, 2.0f, CHUNK_SIZE_X * WORLD_SIZE_X*2, CHUNK_SIZE_Z * WORLD_SIZE_Z*2) * CHUNK_SIZE_Y / 2);
		if (pos.y == noise + 1) {
			if (Random::getFloat(0.0f, 1.0f) < .01f)
				return BlockType::DiamondBlock;
		}
		else if (pos.y == noise)
			return BlockType::Grass;
		else if (pos.y < noise - 4)
			return BlockType::Stone;
		else if (pos.y < noise)
			return BlockType::Dirt;
		return BlockType::Air;
	}
	Block& World::getBlock(BlockType type) {
		return blocks[type];
	}
}