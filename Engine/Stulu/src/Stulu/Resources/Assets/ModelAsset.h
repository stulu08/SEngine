#pragma once
#include "Stulu/Resources/AssetData.h"
#include "Stulu/Resources/AssetsManager.h"

#include "MeshAsset.h"
#include "MaterialAsset.h"

struct aiScene;
struct aiNode;

namespace Stulu {
	struct ModelImportSettings {
		bool OptimizeMeshes = true;
		bool RemoveRedundancies = false;
		bool HandleInvalidData = true;
		bool ScaleVertices = false;

		bool LoadMeshes = true;
		bool LoadMaterials = true;
		float ScaleMod = 1.0f;
	};

	struct ModelMeshNode {
		ModelMeshNode() = default;
		ModelMeshNode(const ModelMeshNode&) = delete;
		ModelMeshNode& operator=(const ModelMeshNode&) = delete;
		ModelMeshNode(ModelMeshNode&&) = default;
		ModelMeshNode& operator=(ModelMeshNode&&) = default;

		std::vector<MaterialAsset> materials;
		MeshAsset mesh;
	};
	struct ModelNode {
		ModelNode() = default;
		ModelNode(const ModelNode&) = delete;
		ModelNode& operator=(const ModelNode&) = delete;
		ModelNode(ModelNode&&) = default;
		ModelNode& operator=(ModelNode&&) = default;

		glm::vec3 Position = glm::vec3(.0f), Scale = glm::vec3(1.0f);
		glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		std::string name;
		ModelMeshNode mesh;
		std::vector<ModelNode> children;
	};

	// Models are always loaded, it is suggested to extract them to seperate assets
	class STULU_API SharedModelAssetData : public SharedAssetData {
	public:
		SharedModelAssetData(std::nullptr_t)
			: SharedAssetData(nullptr) {}
		SharedModelAssetData(UUID uuid, const std::string& path)
			: SharedAssetData(uuid, path) {
			Load();
			IncRef();
		}

		virtual std::type_index GetTypeID() const override { return typeid(SharedModelAssetData); }
		virtual const char* GetTypeName() const override { return "ModelAsset"; }
		virtual bool Loaded() const override { return true; }
		virtual void Unload() override { 
			m_rootNode = ModelNode();
			m_materialPool.clear();
		}
		virtual bool Load() override;
		virtual bool Save() const override { return false; }
		virtual bool HasExtension(const std::string& extension) const override {
			return extension == ".glb" || extension == ".gltf" || extension == ".obj" || extension == ".fbx" || extension == ".blend";
		};

		ModelImportSettings ReadSettings();
		bool SaveSetting(ModelImportSettings settings) const;

		inline ModelNode& GetRootNode() { return m_rootNode; }
	private:
		ModelNode m_rootNode;
		std::vector<MaterialAsset> m_materialPool;
		std::vector<MeshAsset> m_meshPool;

		inline Stulu::UUID BuildSubAssetUUID(const std::string& subName) {
			return UUID(std::to_string(GetUUID()) + "@" + subName);
		}

		ModelNode ProcessNode(const aiNode* node, const aiScene* scene, const ModelImportSettings& settings);
		void LoadMaterialPool(const aiScene* scene, const std::string& RootPath);
	};

	using ModelAsset = AssetHandle<SharedModelAssetData>;
}