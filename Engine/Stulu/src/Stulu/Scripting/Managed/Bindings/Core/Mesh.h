#pragma once
namespace StuluBindings {
	struct SubMeshDescription {
		uint32_t indexOffset;
		uint32_t indexCount;
		uint32_t vertexOffset;
		Stulu::Mono::String name;
	};

	class Mesh {
	public:
		static inline MonoString* GetName(uint64_t id) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (asset.IsValid())
				return Stulu::Mono::String::New(getCoreDomain(), asset->GetName());

			return Stulu::Mono::String::New(getCoreDomain(), "");
		}

		static inline MonoString* GetNameInternal(uint64_t id) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return Stulu::Mono::String::New(getCoreDomain(), "");

			return Stulu::Mono::String::New(getCoreDomain(), asset->GetName());
		}
		static inline void SetNameInternal(uint64_t id, Stulu::Mono::String name) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return;

			asset->SetName(name.ToUtf8());
		}
		static inline int32_t GetSubMeshCountInternal(uint64_t id) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return 0;

			return (int32_t)asset->GetSubmeshes().size();
		}
		static inline void GetSubMeshInternal(uint64_t id, SubMeshDescription* mesh, int32_t index) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return;

			Stulu::MeshSubmesh subMesh = asset->GetSubmeshes()[id];

			mesh->indexOffset = subMesh.indexOffset;
			mesh->indexCount = subMesh.indexCount;
			mesh->vertexOffset = subMesh.vertexOffset;
			mesh->name = Mono::String::New(getCoreDomain(), subMesh.name.c_str());
		}
		static inline void SetSubMeshInternal(uint64_t id, SubMeshDescription* mesh, int32_t index) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return;

			auto& subMeshes = asset->GetSubmeshes();

			if (mesh->indexCount == 0) {
				subMeshes.erase(subMeshes.begin() + id);
				return;
			}

			Stulu::MeshSubmesh& subMesh = asset->GetSubmeshes()[id];

			subMesh.indexOffset = mesh->indexOffset;
			subMesh.indexCount = mesh->indexCount;
			subMesh.vertexOffset = mesh->vertexOffset;
			subMesh.name = mesh->name.ToUtf8();
		}
		static inline void SetVerticesInternal(uint64_t id, NativeArray<Stulu::Vertex>* vertices) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return;

			asset->SetVertices(vertices->DataPtr, vertices->Length);
		}
		static inline void GetVerticesInternal(uint64_t id, NativeArray<Stulu::Vertex>* vertices) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return;

			vertices->DataPtr = (Vertex*)asset->GetVertices();
			vertices->Length = asset->GetVerticesCount();
		}
		static inline void SetIndicesInternal(uint64_t id, NativeArray<uint32_t>* indices) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return;

			asset->SetIndices(indices->DataPtr, (uint32_t)indices->Length);
		}
		static inline void GetIndicesInternal(uint64_t id, NativeArray<uint32_t>* indices) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return;

			indices->DataPtr = (uint32_t*)asset->GetIndices().data();
			indices->Length = asset->GetIndices().size();
		}
		static inline void CalculateBoundsInternal(uint64_t id) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return;
			asset->CalculateBounds();
		}
		static inline void CalculateNormalsInternal(uint64_t id, bool upload) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return;
			asset->CalculateNormals(upload);
		}
		static inline void CreateVertexMeshInternal(uint64_t id) {
			using namespace Stulu;
			MeshAsset asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(id);
			if (!asset.IsValid())
				return;
			asset->UploadIndexBuffer();
			asset->UploadVertexBuffer(Stulu::Mesh::DefaultVertexLayout());
		}
	};
}