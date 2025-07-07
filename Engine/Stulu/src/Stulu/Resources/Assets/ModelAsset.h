#pragma once
#include "Stulu/Resources/AssetData.h"
#include "Stulu/Resources/AssetsManager.h"

#include "MeshAsset.h"
#include "MaterialAsset.h"

struct aiScene;
struct aiNode;

namespace Stulu {
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
		}

		virtual std::type_index GetTypeID() const override { return typeid(SharedModelAssetData); }
		virtual const char* GetTypeName() const override { return "ModelAsset"; }
		virtual bool Loaded() const override { return true; }
		virtual void Unload() override { 
			m_rootNode = ModelNode();
		}
		virtual bool Load() override;
		virtual bool Save() const override { return false; }
		virtual bool HasExtension(const std::string& extension) const override {
			return extension == ".glb" || extension == ".gltf" || extension == ".obj" || extension == ".fbx";
		};
	private:
		ModelNode m_rootNode;

		inline Stulu::UUID BuildSubAssetUUID(const std::string& subName) {
			return UUID(GetPath() + "@" + subName);
		}

		// force move construct return
		ModelNode ProcessNode(const aiNode* node, const aiScene* scene);
	};
}