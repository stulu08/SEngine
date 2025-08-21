#pragma once
#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/Material/TestMaterial.h"
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	class DrawList {
	public:
		using MaterialList = std::unordered_map<UUID, DrawCommand>;
		using MeshList = std::unordered_map<UUID, MaterialList>;

		DrawList() {
			m_meshes.reserve(256);
		}

        inline void AddCommand(
            Mesh* mesh,
            TestMaterial* material,
            SharedModelDataBuffer* modelData,
            uint32_t modelDataSize,
            uint32_t meshIndex = 0
        ) {
            UUID meshId = mesh->GetUUID();
            UUID matId = material->GetUUID();

            auto& matList = m_meshes[meshId];
            auto it = matList.find(matId);

            if (it != matList.end()) {
                // Merge into existing command (instancing)
                DrawCommand& cmd = it->second;
                cmd.count++;
                // Append new model data (if applicable)
                // Assuming your SharedModelDataBuffer supports batched append
                cmd.modelData->Append(modelData, modelDataSize);
            }
            else {
                // Create a new command
                DrawCommand cmd;
                cmd.mesh = mesh;
                cmd.material = material;
                cmd.stencilValue = stencilValue;
                cmd.cullMode = cullMode;
                cmd.count = 1;
                cmd.modelData = modelData;
                cmd.modelDataSize = modelDataSize;
                cmd.meshIndex = meshIndex;
                matList.emplace(matId, std::move(cmd));
            }
        }

		inline const MeshList& GetMeshes() const { return m_meshes; }
		inline void Clear() { m_meshes.clear(); }

	private:
		MeshList m_meshes;
	};
}