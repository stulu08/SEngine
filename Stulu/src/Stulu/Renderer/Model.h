#pragma once
#include "Stulu/Renderer/Mesh.h"
#include <Stulu/Renderer/Shader.h>

struct aiNode;
struct aiMesh;
struct aiScene;

namespace Stulu {
	class Model
	{
	public:
		Model(const std::string& path) {
			load(path);
		}
		Model(Mesh& mesh) {
			meshes.push_back(mesh);
		}
		Model() {
		}
		void submitToRenderer(const Ref<Shader>& shader, const glm::mat4& transform);
		const uint32_t getVerticesCount() { return m_verticesCount; }
		const uint32_t getIndicesCount() { return m_indicesCount; }
		const uint32_t getTriangleCount() { return m_triangleCount; }

		const std::vector<Mesh> getMeshes() { return meshes; }
		const std::vector<std::string> getMesheNames() { return MeshNames; }
	private:
		std::vector<Mesh> meshes;
		std::vector <std::string> MeshNames;
		std::string directory = "";
		uint32_t m_verticesCount = 0;
		uint32_t m_indicesCount = 0;
		uint32_t m_triangleCount = 0;
		void load(const std::string& path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	};
}

