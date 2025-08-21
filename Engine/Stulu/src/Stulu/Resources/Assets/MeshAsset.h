#pragma once
#include "Stulu/Resources/AssetData.h"
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Renderer/Mesh.h"

namespace Stulu {
	/*
	Mesh data will be stored inside an smesh file, after loading the mesh
	[ magic value (sizeof(MagicValue)) ]
	[ header size (sizeof(uint32_t)) ]
	[ 
		header (YAML data about the mesh) 
		VertexCount: 1024
		IndexCount: 2048
		VertexLayout:
			- a_pos: 
				- Type: Float3
			- a_normal:
				- Type: Float3
				- Normalized: true
		SubMeshes:
			- Car:
				- IndexCount: 1024
				- IndexOffset: 0
				- VertexOffset: 0
			- Tires:
				- IndexCount: 64
				- IndexOffset: 1024
				- VertexOffset: 624

		# If DefaultLayout or DefaultSkinnedLayout is true VertexLayout will be ignored
	]
	[ binary blob of vertex data (size = layoutSize * VertexCount) ]
	[ binary blob of index data (size = sizeof(uint32_t) * IndexCount) ]
	*/

	class STULU_API SharedMeshAssetData : public SharedAssetData {
	public:
		SharedMeshAssetData(std::nullptr_t)
			: SharedAssetData(nullptr) {}
		SharedMeshAssetData(UUID uuid, const std::string& path) 
			: SharedAssetData(uuid, path) {}
		SharedMeshAssetData(UUID uuid, Ref<Mesh> mesh)
			: SharedAssetData(uuid, MemoryAssetPath), m_mesh(mesh) {}

		virtual std::type_index GetTypeID() const override { return typeid(SharedMeshAssetData); }
		virtual const char* GetTypeName() const override { return "MeshAsset"; }
		virtual bool HasExtension(const std::string& extension) const override { return extension == ".smesh"; };
		virtual bool Loaded() const override { return m_mesh != nullptr; }
		virtual void Unload() override { m_mesh.reset(); }

		virtual bool Load() override {
			if (IsMemoryLoaded())
				return false;
			return LoadFromPath(GetPath());
		}

		virtual bool Save() const override {
			if (!m_mesh || IsMemoryLoaded())
				return false;
			return SaveToFile(GetPath());
		}

		bool SaveToFile(const std::string& fileDest) const;

		void SetMeshRefrence(const Ref<Mesh> mesh) { m_mesh = mesh; }

		using NativeType = Mesh;
		NativeType* GetNative() { return m_mesh.get(); }
		const NativeType* GetNative() const { return m_mesh.get(); }
	private:
		Ref<Mesh> m_mesh = nullptr;

		bool LoadFromPath(const std::string& path);

		inline static const char MagicValue[5] = { 'S', 'M', 'S', 'H', '1' };
	};
	using MeshAsset = AssetHandle<SharedMeshAssetData>;
}