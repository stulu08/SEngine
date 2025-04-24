#pragma once
#include "Stulu/Resources/AssetData.h"
#include "Stulu/Resources/AssetsManager.h"

#include "MeshAsset.h"
#include "MaterialAsset.h"

namespace Stulu {
	// Models are not stored, they can be extracted into an 
	class SharedModelAssetData : public SharedAssetData {
	public:
		SharedModelAssetData(std::nullptr_t)
			: SharedAssetData(nullptr) {}
		SharedModelAssetData(UUID uuid, const std::string& path)
			: SharedAssetData(uuid, path) {}

		virtual std::type_index GetTypeID() const override { return typeid(SharedModelAssetData); }
		virtual const char* GetTypeName() const override { return "ModelAsset"; }
		virtual bool Loaded() const override { return true; }
		virtual void Unload() override { }
		virtual bool Load() override { return true; }
		virtual bool Save() const override { return true; }
		virtual bool HasExtension(const std::string& extension) const override {
			return extension == ".glb" || extension == ".gltf" || extension == ".obj" || extension == ".fbx";
		};

		// Will extract the meshes and materials
		void Extract(const std::string& path = "");
	};
}