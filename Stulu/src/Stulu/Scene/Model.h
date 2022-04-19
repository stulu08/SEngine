#pragma once
#include "Stulu/Renderer/Mesh.h"

struct aiNode;
struct aiMesh;
struct aiScene;
template<typename>
class aiMatrix4x4t;
typedef aiMatrix4x4t<float> aiMatrix4x4;

namespace Stulu {
	struct MeshAsset {
		std::string name;
		Ref<Mesh> mesh;
		UUID uuid = UUID();

		bool hasMesh = false;
		UUID parentMeshAsset = UUID::null;
		glm::mat4 transform = glm::mat4(1.0f);

		UUID material = UUID::null;

		MeshAsset() = default;
		MeshAsset(const MeshAsset&) = default;
	};
	class STULU_API Model
	{
	public:
		Model(const std::string& path) {
			load(path);
		}
		Model(Ref<Mesh>& mesh, const std::string& name = "new Mesh") {
			meshes.push_back({ name, mesh });
		}

		std::vector<MeshAsset>& getMeshes() { return meshes; }
	private:
		std::vector<MeshAsset> meshes;
		void load(const std::string& path);
		
		void processNode(aiNode* node, const aiScene* scene, UUID& parent = UUID::null);
		
		static Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		static SubMesh processSubMesh(aiMesh* mesh, const aiScene* scene);
	};
}

