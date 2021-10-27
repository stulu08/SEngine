#include "st_pch.h"
#include "Resources.h"
#include <Stulu/Renderer/Model.h>

namespace Stulu {
	void Resources::load() {
		loadCubeMesh();
		loadPlaneMesh();
		loadSphereMesh();
		loadSkyBoxMesh();
	}

	Ref<Mesh>& Resources::getCubeMesh() {
		return s_cubeMesh;
	}
	Ref<Mesh>& Resources::getPlaneMesh() {
		return s_planeMesh;
	}
	Ref<Mesh>& Resources::getSphereMesh() {
		return s_sphereMesh;
	}
	Ref<Mesh>& Resources::getSkyBoxMesh() {
		return s_skyBoxMesh;
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
		s_cubeMesh = createRef<Mesh>(vertices, indices);
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
		s_planeMesh = createRef<Mesh>(vertices, indices);
	}
	void Resources::loadSphereMesh() {
		s_sphereMesh = createRef<Mesh>(Model("Stulu/assets/Meshes/sphere.obj").getMeshes()[0]);
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
		s_skyBoxMesh = createRef<Mesh>(vertices, indices);
	}
}
