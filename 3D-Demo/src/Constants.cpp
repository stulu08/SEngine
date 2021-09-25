#include "Constants.h"

glm::vec3 BlockData::verts[8] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(1.0f, 0.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(0.0f, 1.0f, 1.0f),
};
glm::vec2 BlockData::uvs[4] = {
	glm::vec2(0.0f, 0.0f),
	glm::vec2(0.0f, 1.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(1.0f, 1.0f)
};
//Back - Front - Top - Bottom - Left - Right
uint32_t BlockData::tris[6][4] = {
	{0, 3, 1, 2},
	{5, 6, 4, 7},
	{3, 7, 2, 6},
	{1, 5, 0, 4},
	{4, 7, 0, 3},
	{1, 2, 5, 6},
};
//Back - Front - Top - Bottom - Left - Right
glm::vec3 BlockData::normals[6]{
	glm::vec3(.0f, .0f, .0f),
	glm::vec3(.0f, .0f, .0f),
	glm::vec3(.0f, .0f, .0f),
	glm::vec3(.0f, .0f, .0f),
	glm::vec3(.0f, .0f, .0f),
	glm::vec3(.0f, .0f, .0f),
};
glm::vec3 BlockData::faceChecks[6] = {

	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),

};