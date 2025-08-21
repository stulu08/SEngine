#pragma once
#include "AssetWrapper.h"

namespace StuluBindings {
	struct SubMeshDescription {
		uint32_t indexOffset;
		uint32_t indexCount;
		uint32_t vertexOffset;
		Stulu::Mono::String name;
	};

	class Mesh : public AssetWrapper<Stulu::MeshAsset> {
	public:
		static inline MonoString* GetNameInternal(uint64_t id) {
			if (auto asset = SaveGetAsset(id)) {
				return Stulu::Mono::String::New(getCoreDomain(), asset->GetName());
			}
			return Stulu::Mono::String::New(getCoreDomain(), "");
		}

		static inline void SetNameInternal(uint64_t id, Stulu::Mono::String name) {
			if (auto asset = SaveGetAsset(id)) {
				asset->SetName(name.ToUtf8());
			}
		}
		static inline int32_t GetSubMeshCountInternal(uint64_t id) {
			if (auto asset = SaveGetAsset(id)) {
				return (int32_t)asset->GetSubmeshes().size();
			}
			return 0;
		}
		static inline void GetSubMeshInternal(uint64_t id, SubMeshDescription* mesh, int32_t index) {
			if (auto asset = SaveGetAsset(id)) {
				Stulu::MeshSubmesh subMesh = asset->GetSubmeshes()[index];

				mesh->indexOffset = subMesh.indexOffset;
				mesh->indexCount = subMesh.indexCount;
				mesh->vertexOffset = subMesh.vertexOffset;
				mesh->name = Stulu::Mono::String::New(getCoreDomain(), subMesh.name.c_str());
			}
		}
		static inline void SetSubMeshInternal(uint64_t id, SubMeshDescription* mesh, int32_t index) {
			if (auto asset = SaveGetAsset(id)) {
				auto& subMeshes = asset->GetSubmeshes();
				if (mesh->indexCount == 0) {
					subMeshes.erase(subMeshes.begin() + index);
					return;
				}

				Stulu::MeshSubmesh& subMesh = asset->GetSubmeshes()[index];

				subMesh.indexOffset = mesh->indexOffset;
				subMesh.indexCount = mesh->indexCount;
				subMesh.vertexOffset = mesh->vertexOffset;
				subMesh.name = mesh->name.ToUtf8();
			}
		}
		static inline void SetVerticesInternal(uint64_t id, NativeArray<Stulu::Vertex>* vertices) {
			if (auto asset = SaveGetAsset(id)) {
				asset->SetVertices(vertices->DataPtr, vertices->Length);
			}
		}
		static inline void GetVerticesInternal(uint64_t id, NativeArray<Stulu::Vertex>* vertices) {
			if (auto asset = SaveGetAsset(id)) {
				vertices->DataPtr = (Stulu::Vertex*)asset->GetVertices();
				vertices->Length = asset->GetVerticesCount();
			}
		}
		static inline void SetIndicesInternal(uint64_t id, NativeArray<uint32_t>* indices) {
			if (auto asset = SaveGetAsset(id)) {
				asset->SetIndices(indices->DataPtr, (uint32_t)indices->Length);
			}
		}
		static inline void GetIndicesInternal(uint64_t id, NativeArray<uint32_t>* indices) {
			if (auto asset = SaveGetAsset(id)) {
				indices->DataPtr = (uint32_t*)asset->GetIndices().data();
				indices->Length = asset->GetIndices().size();
			}
		}
		static inline void CalculateBoundsInternal(uint64_t id) {
			if (auto asset = SaveGetAsset(id)) {
				asset->CalculateBounds();
			}
		}
		static inline void CalculateNormalsInternal(uint64_t id, bool upload) {
			if (auto asset = SaveGetAsset(id)) {
				asset->CalculateNormals();
			}
		}
		static inline void CreateVertexMeshInternal(uint64_t id) {
			if (auto asset = SaveGetAsset(id)) {
				asset->UploadIndexBuffer();
				asset->UploadVertexBuffer(Stulu::Mesh::DefaultVertexLayout());
			}
		}
	};
}