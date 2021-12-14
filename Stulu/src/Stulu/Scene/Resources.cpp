#include "st_pch.h"
#include "Resources.h"
#include "Stulu/Renderer/Model.h"
#include "Stulu/Scene/AssetsManager.h"

namespace Stulu {
	void Resources::load() {
		loadCubeMesh();
		loadPlaneMesh();
		loadSphereMesh();
		loadSkyBoxMesh();
		loadBlackTexture();
	}

	Ref<Mesh> Resources::getCubeMesh() {
		return std::any_cast<MeshAsset>(AssetsManager::get(UUID(401)).data).mesh;
	}
	Ref<Mesh> Resources::getPlaneMesh() {
		return std::any_cast<MeshAsset>(AssetsManager::get(UUID(402)).data).mesh;
	}
	Ref<Mesh> Resources::getSphereMesh() {
		return std::any_cast<MeshAsset>(AssetsManager::get(UUID(404)).data).mesh;
	}
	Ref<Mesh> Resources::getSkyBoxMesh() {
		return std::any_cast<MeshAsset>(AssetsManager::get(UUID(403)).data).mesh;
	}
	Ref<Texture2D> Resources::getBlackTexture() {
		return std::any_cast<Ref<Texture2D>>(AssetsManager::get(UUID(301)).data);
	}
	MeshAsset& Resources::getCubeMeshAsset() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(401)).data);
	}
	MeshAsset& Resources::getPlaneMeshAsset() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(402)).data);
	}
	MeshAsset& Resources::getSphereMeshAsset() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(404)).data);
	}
	MeshAsset& Resources::getSkyBoxMeshAsset() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(403)).data);
	}
	void Resources::loadCubeMesh() {
		std::vector<Vertex> vertices{
			//top
			{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//bottom														 		 
			{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//right															 		 
			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//left															 		 
			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//front															 		 
			{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},
			//back															 		 
			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
		};
		std::vector<uint32_t> indices{
			//top
			2,1,0,
			0,3,2,
			//bottom
			4,5,6,
			6,7,4,
			//right
			10,9,8,
			8,11,10,
			//left
			12,13,14,
			14,15,12,
			//front
			16,17,18,
			18,19,16,
			//back
			22,21,20,
			20,23,22
		};
		Ref<Mesh> m = createRef<Mesh>(vertices, indices);
		AssetsManager::update(UUID(401), Asset{ AssetType::Mesh, MeshAsset{"Cube",m,UUID(401),true},"",UUID(401) });
	}
	void Resources::loadPlaneMesh() {
		std::vector<Vertex> vertices{
			{glm::vec3(-0.5f,  0.0f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f,  0.0f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f,  0.0f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.0f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
		};
		std::vector<uint32_t> indices{
			2,1,0,
			0,3,2,
		};
		Ref<Mesh> m = createRef<Mesh>(vertices, indices);
		AssetsManager::update(UUID(402), Asset{ AssetType::Mesh, MeshAsset{"Plane",m,UUID(402),true},"",UUID(402), });
	}
	void Resources::loadSphereMesh() {
	}
	void Resources::loadSkyBoxMesh() {
		std::vector<Vertex> vertices{
			//top
			{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//bottom														 		 
			{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//right															 		 
			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//left															 		 
			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//front															 		 
			{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},
			//back															 		 
			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
		};
		std::vector<uint32_t> indices{
			//top
			0,1,2,
			2,3,0,
			//bottom
			6,5,4,
			4,7,6,
			//right
			8,9,10,
			10,11,8,
			//left
			14,13,12,
			12,15,14,
			//front
			18,17,16,
			16,19,18,
			//back
			20,21,22,
			22,23,20
		};
		Ref<Mesh> m = createRef<Mesh>(vertices, indices);
		AssetsManager::update(UUID(403), Asset{ AssetType::Mesh, MeshAsset{"SkyBox Mesh",m,UUID(403),true},"",UUID(403),});

	}
	void Resources::loadBlackTexture() {
		Ref<Texture2D> tex = Texture2D::create(1, 1);
		uint32_t datawhite = 0xffffffff;
		tex->setData(&datawhite, sizeof(uint32_t));
		AssetsManager::update(UUID(301), Asset{ AssetType::Texture2D,tex,"",UUID(301), });
	}
}
