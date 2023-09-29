#pragma once
#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/RenderAPI.h"
#include "Stulu/Scene/Material.h"

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
		mutable UUID uuid = UUID();

		bool hasMesh = false;
		UUID parentMeshAsset = UUID::null;
		glm::mat4 transform = glm::mat4(1.0f);

		std::vector<int32_t> materialIDs{};
		std::vector<UUID> materials{};

		CullMode cullMode = CullMode::Back;

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
		std::unordered_map<uint32_t, Ref<Material>>& getMaterials() { return materials; }
		const std::string& getDirectory() { return directory; }
	private:
		std::string directory;
		std::vector<MeshAsset> meshes;
		std::unordered_map<uint32_t, Ref<Material>> materials;
		void load(const std::string& path);
		
		void processNode(aiNode* node, const aiScene* scene, UUID& parent = UUID::null);
		
		bool loadMaterial(const aiScene* scene, uint32_t material);

		static Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		static SubMesh processSubMesh(aiMesh* mesh, const aiScene* scene);
	};
}

