#pragma once
#include "Stulu.h"
#include "Blocks/Blocks.h"
#include "App.h"
namespace Demo {
#define WORLD_SIZE_X 8
#define WORLD_SIZE_Z 8
	using ChunkCoords = glm::i32vec2;
	class Chunk;

	class World : public Behavior {
	public:
		std::unordered_map<ChunkCoords, GameObject> loadedChunks;
		virtual void onStart() override;
		virtual void onUpdate() override;
		GameObject createChunk();

		Chunk* getChunk(const ChunkCoords& coords);

		static void loadBlocks();
		static bool isTransparent(BlockType type);
		static BlockType getBlockFromPerlin(const glm::vec3& pos);
		static Block& getBlock(BlockType type);
	private:
		static inline std::unordered_map<BlockType, Block> blocks;
		static inline Ref<Texture2D> textureAtals;
	};
}