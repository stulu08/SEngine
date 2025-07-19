#pragma once
#include "Stulu/Resources/AssetData.h"
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Resources/Assets/MeshAsset.h"

namespace Stulu {
	class STULU_API TestMaterial;
	class STULU_API SharedMaterialAssetData : public SharedAssetData {
	public:
		SharedMaterialAssetData(std::nullptr_t)
			: SharedAssetData(nullptr) {}
		SharedMaterialAssetData(UUID uuid, const std::string& path)
			: SharedAssetData(uuid, path) {}
		SharedMaterialAssetData(UUID uuid, Ref<TestMaterial> material)
			: SharedAssetData(uuid, MemoryAssetPath), m_material(material) {}

		virtual std::type_index GetTypeID() const override { return typeid(SharedMaterialAssetData); }
		virtual const char* GetTypeName() const override { return "MaterialAsset"; }
		virtual void Unload() override { m_material.reset(); }
		virtual bool Loaded() const override { return m_material != nullptr; }
		virtual bool HasExtension(const std::string& extension) const override { return extension == ".smaterial"; };
		virtual bool Load() override;
		virtual bool Save() const override;

		virtual void SetMaterial(Ref<TestMaterial> material) {
			m_material = material;
		}

		using NativeType = TestMaterial;
		NativeType* GetNative() { return m_material.get(); }
		const NativeType* GetNative() const { return m_material.get(); }
	private:
		Ref<TestMaterial> m_material = nullptr;
	};

	using MaterialAsset = AssetHandle<SharedMaterialAssetData>;
}