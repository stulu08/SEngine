#pragma once
#include "World.h"

namespace Demo {
#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 128
#define CHUNK_SIZE_Z 16
#define CHUNK_QUAD_COUNT CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z
	struct Vertex {
		glm::vec3 pos = glm::vec3(.0f);
		glm::vec3 nornaml = glm::vec3(.0f);
		glm::vec2 texCoords = glm::vec2(.0f);
		float mettalic = .0f;
		float roughness = .0f;
		float ao = .2f;
	};
	enum class Face {
		Top, Bottom, Left, Right, Front, Back
	};
	class Chunk : public Behavior {
	public:
		static void loadShader();

		void generate(const ChunkCoords& chunkCoords);
		void update();
		virtual void onRender(Timestep ts) override;

		void setBlock(glm::i32vec3 chunkPos, BlockType type);

		BlockType faceCheck(int x, int y, int z, Face face);

		static glm::vec3 chunkCoordsToWorld(const ChunkCoords& chunk, const glm::vec3& insideChunkCoor = glm::vec3(.0f));
		static ChunkCoords worldCoordsToChunk(const glm::vec3& world);

		BlockType blockMap[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];
		bool enabled = false;
		bool generated = false;
		ChunkCoords chunkCoords;
		Mesh mesh;
		World* world;
	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		static inline BufferLayout s_bufferLayout{
			{ Stulu::ShaderDataType::Float3, "a_pos" },
			{ Stulu::ShaderDataType::Float3, "a_normal" },
			{ Stulu::ShaderDataType::Float2, "a_texCoord" },
			{ Stulu::ShaderDataType::Float, "a_metallic" },
			{ Stulu::ShaderDataType::Float, "a_roughness" },
			{ Stulu::ShaderDataType::Float, "a_ao" },
		};
		static inline Ref<Shader> shader = nullptr;
	};
}