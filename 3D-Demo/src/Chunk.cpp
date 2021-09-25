#include "Chunk.h"

Chunk::Chunk(glm::vec2 pos, World* world)
	:world(world){

	worldPos = toWorldPos(pos);

	generate();
}

void Chunk::generate() {
	populate();
	createMesh();
	reCalcMesh();
}
void Chunk::populate() {
	for (int y = 0; y < ChunkData::height; y++) {
		for (int x = 0; x < ChunkData::width; x++) {
			for (int z = 0; z < ChunkData::depth; z++) {
				blockMap[x][y][z] = true;
			}
		}
	}
}
void Chunk::createMesh() {
	for (int y = 0; y < ChunkData::height; y++) {
		for (int x = 0; x < ChunkData::width; x++) {
			for (int z = 0; z < ChunkData::depth; z++) {
				glm::vec3 pos((float)x, (float)y, (float)z);
				genBlock(pos);
			}
		}
	}
}
void Chunk::genBlock(glm::vec3& pos) {
	for (int p = 0; p < 6; p++) {
		if (!checkBlock(pos + BlockData::faceChecks[p])) {
			for (int i = 0; i < 4; i++)
				vertices.push_back({ pos + BlockData::verts[BlockData::tris[p][i]], BlockData::normals[p], BlockData::uvs[i] });

			indices.push_back(vertexIndex);
			indices.push_back(vertexIndex + 1);
			indices.push_back(vertexIndex + 2);
			indices.push_back(vertexIndex + 2);
			indices.push_back(vertexIndex + 1);
			indices.push_back(vertexIndex + 3);
			vertexIndex += 4;
		}
	}
}
void Chunk::reCalcMesh() {
	mesh = Mesh(vertices, indices);
}


bool Chunk::checkBlock(glm::vec3 pos) {

	int x = (int)std::floorf(pos.x);
	int y = (int)std::floorf(pos.y);
	int z = (int)std::floorf(pos.z);
	if (x < 0 || x > ChunkData::width - 1 || y < 0 || y > ChunkData::height - 1 || z < 0 || z > ChunkData::depth - 1)
		return false;
	return blockMap[x, y, z];
}
void Chunk::draw(const Ref<Shader>& shader) {
	shader->bind();
	shader->setFloat3("u_worldPos", worldPos);
	mesh.getVertexArray()->bind();
	RenderCommand::drawIndexed(mesh.getVertexArray(), 0);
}