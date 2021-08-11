#include "Terrain.h"

#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>


const uint32_t Chunk::s_size = 16;
const int Chunk::s_viewDistance = 6;
const int Chunk::s_deleteDistance = 32;
const float Chunk::s_heightScale = 3.5f;

Chunk::Chunk() {
	hidden = true;
	generated = false;
}
Chunk::Chunk(glm::vec2 chunkCoords) {
	Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
	Stulu::Ref<Stulu::IndexBuffer> indexBuffer;

	glm::vec2 offset(chunkCoords * (float)s_size);
	float verices [(s_size + 1) * (s_size + 1) * 3];
	for (int i = 0, z = 0; z <= s_size; z++)
	{
		for (int x = 0; x <= s_size; x++)
		{
			float y = Stulu::Math::perlinAccumalatedNosie(x + offset.x, z + offset.y, 64, .75f, s_size, s_size, false);
			verices[i] = x + .0f;
			verices[i + 1] = y * s_heightScale;
			verices[i + 2] = z + .0f;
			i+=3;
		}
	}
	uint32_t indices[s_size * s_size * 6];

	int vert = 0;
	int tris = 0;
	for (int z = 0; z < s_size; z++)
	{
		for (int x = 0; x < s_size; x++)
		{
			indices[tris + 0] = vert + 0;
			indices[tris + 1] = vert + s_size + 1;
			indices[tris + 2] = vert + 1;
			indices[tris + 3] = vert + 1;
			indices[tris + 4] = vert + s_size + 1;
			indices[tris + 5] = vert + s_size + 2;

			vert++;
			tris += 6;
		}
		vert++;
	}
	vertexArray = Stulu::VertexArray::create();
	vertexBuffer = Stulu::VertexBuffer::create(sizeof(verices), verices);
	vertexBuffer->setLayout(layout);
	vertexArray->addVertexBuffer(vertexBuffer);
	indexBuffer = Stulu::IndexBuffer::create(sizeof(indices) / sizeof(uint32_t), indices);
	vertexArray->setIndexBuffer(indexBuffer);
	transform.setPos(glm::vec3(chunkCoords.x * s_size, .0f, chunkCoords.y * s_size));
	generated = true;
	coords = chunkCoords;
}
void Chunk::update(glm::vec3 player) {
	hidden = glm::distance(transform.getPos(), player) > s_viewDistance * s_size;
}
void TerrainLayer::onAttach() {
	m_shaderLib.load("assets/Shaders/heightColor.glsl");
	m_shaderLib.get("heightColor")->bind();
	m_shaderLib.get("heightColor")->setFloat("u_maxHeight", Chunk::s_heightScale);
	Stulu::Math::setPerlinSeed(Stulu::Random::getInt(0, 9999999));
	genChunks(m_cameraController.getTransform().getPos());

	std::vector<std::string> faces =
	{
		"assets/SkyBox/default/right.jpg",
		"assets/SkyBox/default/left.jpg",
		"assets/SkyBox/default/top.jpg",
		"assets/SkyBox/default/bottom.jpg",
		"assets/SkyBox/default/front.jpg",
		"assets/SkyBox/default/back.jpg"
	};
	Stulu::Renderer::setSkyBox(Stulu::CubeMap::create(faces));
}
void TerrainLayer::onEvent(Stulu::Event& e) {
	m_cameraController.onEvent(e);
}
void TerrainLayer::onImguiRender(Stulu::Timestep timestep) {
	ImGui::Begin("Application");
	ImGui::Text("Deltatime: %.3fs(%dms)", (float)timestep, (int)timestep.getMilliseconds());
	ImGui::Text("FPS: %.2f", 1.0f / timestep);
	ImGui::End();
}
bool TerrainLayer::containsChunk(glm::vec2& Ck) {
	for (int i = 0; i < chunks.size(); i++) {
		if (chunks[i].coords == Ck)
			return true;
	}
	return false;
}
void TerrainLayer::genChunks(glm::vec3& pos) {
	for (int i = 0, x = pos.x / Chunk::s_size - Chunk::s_viewDistance; x < pos.x / Chunk::s_size + Chunk::s_viewDistance; x++)
	{
		for (int y = pos.z / Chunk::s_size - Chunk::s_viewDistance; y < pos.z / Chunk::s_size + Chunk::s_viewDistance; y++) {
			glm::vec2 ck((float)x, (float)y);
			if (!containsChunk(ck))
				chunks.push_back(Chunk(ck));
		}
	}
}
void TerrainLayer::onUpdate(Stulu::Timestep timestep) {
	//update
	m_cameraController.onUpdate(timestep);
	genChunks(m_cameraController.getTransform().getPos());
	for (int i = 0; i < chunks.size(); i++) {
		chunks[i].update(m_cameraController.getTransform().getPos());
		if (chunks[i].hidden && glm::distance(chunks[i].transform.getPos(), m_cameraController.getTransform().getPos()) > Chunk::s_deleteDistance * Chunk::s_size)
			chunks.erase(chunks.begin() + i);
		
	}
	//render
	Stulu::RenderCommand::setClearColor(m_clearColor);
	Stulu::RenderCommand::clear();
	Stulu::Renderer::beginScene(m_cameraController.getCamera(), true);

	m_shaderLib.get("heightColor")->setFloat3("u_camPos", m_cameraController.getTransform().getPos());



	for (int i = 0; i < chunks.size(); i++) {
		if (chunks[i].generated && !chunks[i].hidden) {
			Stulu::Renderer::submit(chunks[i].vertexArray, m_shaderLib.get("heightColor"), chunks[i].transform);
		}
	}
	Stulu::Renderer::endScene();
}