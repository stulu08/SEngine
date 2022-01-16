#include "Chunk.h"

namespace Demo {
	void Chunk::loadShader() {
		shader = Shader::create("assets/chunk.glsl");
	}
	void Chunk::generate(const ChunkCoords& coords) {
		chunkCoords = coords;
		getComponent<TransformComponent>().position = chunkCoordsToWorld(chunkCoords);
		getComponent<GameObjectBaseComponent>().name = glm::to_string(chunkCoords);
		for (int x = 0, i = 0; x < CHUNK_SIZE_X; x++) {
			for (int y = 0; y < CHUNK_SIZE_Y; y++) {
				for (int z = 0; z < CHUNK_SIZE_Z; z++) {
					blockMap[x][y][z] = World::getBlockFromPerlin(chunkCoordsToWorld(chunkCoords,glm::vec3(x,y,z)));
				}
			}
		}
		vertices = std::vector<Vertex>(CHUNK_QUAD_COUNT * 4 * 6);
		int i = 0;
		for (int x = 0; x < CHUNK_SIZE_X; x++) {
			for (int y = 0; y < CHUNK_SIZE_Y; y++) {
				for (int z = 0; z < CHUNK_SIZE_Z; z++) {
					BlockType blockType = blockMap[x][y][z];
					if (blockType == BlockType::Air)
						continue;
					float id = (float)((int)blockType);
					Block& block = World::getBlock(blockType);
					Block::Material& m = block.mat;
					if(World::isTransparent(faceCheck(x,y,z,Face::Top))){
						const glm::vec2* texCoords = block.top->getTexCoords();
						
						vertices[i]		= Vertex{ glm::vec3(x,			y,		z),			glm::vec3(.0f,1.0f,.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x+1.0f,		y,		z),			glm::vec3(.0f,1.0f,.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x+1.0f,		y,		z+1.0f),	glm::vec3(.0f,1.0f,.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x,			y,		z+1.0f),	glm::vec3(.0f,1.0f,.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
					if (World::isTransparent(faceCheck(x,y,z, Face::Bottom))) {
						const glm::vec2* texCoords = block.bottom->getTexCoords();
						vertices[i]		= Vertex{ glm::vec3(x,			y-1.0f,	z+1.0f),	glm::vec3(.0f,-1.0f,.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x+1.0f,		y-1.0f,	z+1.0f),	glm::vec3(.0f,-1.0f,.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x+1.0f,		y-1.0f,	z),			glm::vec3(.0f,-1.0f,.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x,			y-1.0f,	z),			glm::vec3(.0f,-1.0f,.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
					if (World::isTransparent(faceCheck(x,y,z, Face::Left))) {
						const glm::vec2* texCoords = block.left->getTexCoords();
						vertices[i]		= Vertex{ glm::vec3(x,		y-1.0f,		z+1.0f),	glm::vec3(-1.0f,.0f,.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x,		y-1.0f,		z),			glm::vec3(-1.0f,.0f,.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x,		y,			z),			glm::vec3(-1.0f,.0f,.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x,		y,			z+1.0f),	glm::vec3(-1.0f,.0f,.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
					if (World::isTransparent(faceCheck(x,y,z, Face::Right))) {
						const glm::vec2* texCoords = block.right->getTexCoords();
						vertices[i]		= Vertex{ glm::vec3(x+1.0f,	y-1.0f,		z),			glm::vec3(1.0f,.0f,.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x+1.0f,	y-1.0f,		z+1.0f),	glm::vec3(1.0f,.0f,.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x+1.0f,	y,			z+1.0f),	glm::vec3(1.0f,.0f,.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x+1.0f,	y,			z),			glm::vec3(1.0f,.0f,.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
					if (World::isTransparent(faceCheck(x,y,z, Face::Front))) {
						const glm::vec2* texCoords = block.front->getTexCoords();
						vertices[i]		= Vertex{ glm::vec3(x+1.0f,	y-1.0f,		z+1.0f),	glm::vec3(.0f,.0f,1.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x,		y-1.0f,		z+1.0f),	glm::vec3(.0f,.0f,1.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x,		y,			z+1.0f),	glm::vec3(.0f,.0f,1.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x+1.0f,	y,			z+1.0f),	glm::vec3(.0f,.0f,1.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
					if (World::isTransparent(faceCheck(x,y,z, Face::Back))) {
						const glm::vec2* texCoords = block.back->getTexCoords();
						vertices[i]		= Vertex{ glm::vec3(x,		y-1.0f,		z),		glm::vec3(.0f,.0f,-1.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x+1.0f,	y-1.0f,		z),		glm::vec3(.0f,.0f,-1.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x+1.0f,	y,			z),		glm::vec3(.0f,.0f,-1.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x,		y,			z),		glm::vec3(.0f,.0f,-1.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
				}
			}
		}
		indices = std::vector<uint32_t>(CHUNK_QUAD_COUNT * 6 * 6);
		uint32_t offset = 0;
		for (uint32_t i = 0; i < CHUNK_QUAD_COUNT * 6 * 6; i += 6) {
			indices[i + 0] = offset + 2;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 0;

			indices[i + 3] = offset + 0;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 2;
			offset += 4;
		}
		update();
		generated = true;
	}
	void Chunk::update() {
		mesh = Mesh(vertices.data(), sizeof(Vertex) * vertices.size(), indices.data(), indices.size(), s_bufferLayout);
	}
	bool has = false;
	void Chunk::onRender(Timestep ts) {
		if (enabled && !mesh.getVertexArray())
			return;
		if (shader) {
			shader->bind();
		}
		else {
			Resources::getDefaultMaterial()->bind();
		}
		Renderer::submit(mesh.getVertexArray(), glm::translate(glm::mat4(1.0f), Chunk::chunkCoordsToWorld(chunkCoords)));
	}
	void Chunk::setBlock(glm::i32vec3 chunkPos, BlockType type) {
		blockMap[chunkPos.x][chunkPos.y][chunkPos.z] = type;
		vertices = std::vector<Vertex>(CHUNK_QUAD_COUNT * 4 * 6);
		int i = 0;
		for (int x = 0; x < CHUNK_SIZE_X; x++) {
			for (int y = 0; y < CHUNK_SIZE_Y; y++) {
				for (int z = 0; z < CHUNK_SIZE_Z; z++) {
					BlockType blockType = blockMap[x][y][z];
					if (blockType == BlockType::Air)
						continue;
					float id = (float)((int)blockType);
					Block& block = World::getBlock(blockType);
					Block::Material& m = block.mat;
					if (World::isTransparent(faceCheck(x, y, z, Face::Top))) {
						const glm::vec2* texCoords = block.top->getTexCoords();

						vertices[i] = Vertex{ glm::vec3(x,			y,		z),			glm::vec3(.0f,1.0f,.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x + 1.0f,		y,		z),			glm::vec3(.0f,1.0f,.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x + 1.0f,		y,		z + 1.0f),	glm::vec3(.0f,1.0f,.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x,			y,		z + 1.0f),	glm::vec3(.0f,1.0f,.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
					if (World::isTransparent(faceCheck(x, y, z, Face::Bottom))) {
						const glm::vec2* texCoords = block.bottom->getTexCoords();
						vertices[i] = Vertex{ glm::vec3(x,			y - 1.0f,	z + 1.0f),	glm::vec3(.0f,-1.0f,.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x + 1.0f,		y - 1.0f,	z + 1.0f),	glm::vec3(.0f,-1.0f,.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x + 1.0f,		y - 1.0f,	z),			glm::vec3(.0f,-1.0f,.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x,			y - 1.0f,	z),			glm::vec3(.0f,-1.0f,.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
					if (World::isTransparent(faceCheck(x, y, z, Face::Left))) {
						const glm::vec2* texCoords = block.left->getTexCoords();
						vertices[i] = Vertex{ glm::vec3(x,		y - 1.0f,		z + 1.0f),	glm::vec3(-1.0f,.0f,.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x,		y - 1.0f,		z),			glm::vec3(-1.0f,.0f,.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x,		y,			z),			glm::vec3(-1.0f,.0f,.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x,		y,			z + 1.0f),	glm::vec3(-1.0f,.0f,.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
					if (World::isTransparent(faceCheck(x, y, z, Face::Right))) {
						const glm::vec2* texCoords = block.right->getTexCoords();
						vertices[i] = Vertex{ glm::vec3(x + 1.0f,	y - 1.0f,		z),			glm::vec3(1.0f,.0f,.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x + 1.0f,	y - 1.0f,		z + 1.0f),	glm::vec3(1.0f,.0f,.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x + 1.0f,	y,			z + 1.0f),	glm::vec3(1.0f,.0f,.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x + 1.0f,	y,			z),			glm::vec3(1.0f,.0f,.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
					if (World::isTransparent(faceCheck(x, y, z, Face::Front))) {
						const glm::vec2* texCoords = block.front->getTexCoords();
						vertices[i] = Vertex{ glm::vec3(x + 1.0f,	y - 1.0f,		z + 1.0f),	glm::vec3(.0f,.0f,1.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x,		y - 1.0f,		z + 1.0f),	glm::vec3(.0f,.0f,1.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x,		y,			z + 1.0f),	glm::vec3(.0f,.0f,1.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x + 1.0f,	y,			z + 1.0f),	glm::vec3(.0f,.0f,1.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
					if (World::isTransparent(faceCheck(x, y, z, Face::Back))) {
						const glm::vec2* texCoords = block.back->getTexCoords();
						vertices[i] = Vertex{ glm::vec3(x,		y - 1.0f,		z),		glm::vec3(.0f,.0f,-1.0f), texCoords[0],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 1] = Vertex{ glm::vec3(x + 1.0f,	y - 1.0f,		z),		glm::vec3(.0f,.0f,-1.0f), texCoords[1],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 2] = Vertex{ glm::vec3(x + 1.0f,	y,			z),		glm::vec3(.0f,.0f,-1.0f), texCoords[2],m.metallic,m.roughness,m.ambienOcullision };
						vertices[i + 3] = Vertex{ glm::vec3(x,		y,			z),		glm::vec3(.0f,.0f,-1.0f), texCoords[3],m.metallic,m.roughness,m.ambienOcullision };
						i += 4;
					}
				}
			}
		}
		update();
	}
	BlockType Chunk::faceCheck(int x, int y, int z, Face face) {
		switch (face)
		{
		case Face::Top:
			if (y == CHUNK_SIZE_Y - 1)
				return BlockType::Air;
			return blockMap[x][y+1][z];
		case Face::Bottom:
			if (y == 0)
				return BlockType::Air;
			return blockMap[x][y-1][z];
			break;
		case Face::Left:
			if (x == 0)
				return BlockType::Air;
			return blockMap[x-1][y][z];
			break;
		case Face::Right:
			if (x == CHUNK_SIZE_X - 1)
				return BlockType::Air;
			return blockMap[x+1][y][z];
			break;
		case Face::Front:
			if (z == CHUNK_SIZE_Z - 1)
				return BlockType::Air;
			return blockMap[x][y][z+1];
			break;
		case Face::Back:
			if (z == 0)
				return BlockType::Air;
			return blockMap[x][y][z-1];
			break;
		}
		return BlockType::Air;
	}
	glm::vec3 Chunk::chunkCoordsToWorld(const ChunkCoords& chunk, const glm::vec3& insideChunkCoor) {
		return { chunk.x * CHUNK_SIZE_X + insideChunkCoor.x, insideChunkCoor.y, chunk.y * CHUNK_SIZE_Z + insideChunkCoor.z, };
	}
	ChunkCoords Chunk::worldCoordsToChunk(const glm::vec3& world) {
		return { glm::floor(world.x / CHUNK_SIZE_X), glm::floor(world.z / CHUNK_SIZE_Z), };
	}
}