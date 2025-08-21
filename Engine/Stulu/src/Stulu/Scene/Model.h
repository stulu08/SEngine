#pragma once
#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/RenderAPI.h"

struct aiNode;
struct aiMesh;
struct aiScene;
template<typename>
class aiMatrix4x4t;
typedef aiMatrix4x4t<float> aiMatrix4x4;

namespace Stulu {
	/*
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

		static Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
		static Ref<Mesh> processMultiMesh(aiNode* mesh, const aiScene* scene);
	};
	*/
}

