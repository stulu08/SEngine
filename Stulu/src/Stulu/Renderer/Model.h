#pragma once
#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Scene/Components.h"

struct aiNode;
struct aiMesh;
struct aiScene;
template<typename>
class aiMatrix4x4t;
typedef aiMatrix4x4t<float> aiMatrix4x4;

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

		static GameObject loadModel(const std::string& path, Scene* scene, const Ref<Material>& material = nullptr);


		const std::vector<Mesh>& getMeshes() { return meshes; }
	private:
		std::vector<Mesh> meshes;
		std::string directory = "";
		void load(const std::string& path);
		void processNode(aiNode* node, const aiScene* scene);
		static Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		static SubMesh processSubMesh(aiMesh* mesh, const aiScene* scene);

		static GameObject processNode(aiNode* node, const aiScene* scene, Scene* s_scene, const Ref<Material>& material = nullptr);

		static const void decompose(const aiMatrix4x4& aMat, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
	};
}

