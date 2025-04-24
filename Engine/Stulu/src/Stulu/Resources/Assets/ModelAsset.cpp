#include "st_pch.h"
#include "ModelAsset.h"

#ifdef ST_DLL_BUILD
//#define ASSIMP_DLL
#endif
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Stulu {
	static Ref<Mesh> CreateMeshFromNode(const aiNode* node, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<MeshSubmesh> submeshes;

        size_t vertexOffset = 0;
        size_t indexOffset = 0;

        for (size_t meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIndex]];

            MeshSubmesh submesh;
            submesh.name = mesh->mName.C_Str();
            submesh.vertexOffset = static_cast<uint32_t>(vertexOffset);
            submesh.indexOffset = static_cast<uint32_t>(indices.size()); // indexOffset in global index buffer

            const size_t vertexStart = vertices.size();

            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                Vertex vertex;
                glm::vec3 vector;
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.pos = vector;

                if (mesh->HasNormals()) {
                    vector.x = mesh->mNormals[i].x;
                    vector.y = mesh->mNormals[i].y;
                    vector.z = mesh->mNormals[i].z;
                    vertex.normal = glm::normalize(vector);
                }

                if (mesh->mTextureCoords[0]) {
                    glm::vec2 vec;
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.texCoords = vec;
                }
                else {
                    vertex.texCoords = glm::vec2(0.0f, 0.0f);
                }

                vertices.push_back(vertex);
            }

            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(static_cast<uint32_t>(vertexOffset + face.mIndices[j]));
                }
            }

            submesh.indexCount = static_cast<uint32_t>(indices.size() - submesh.indexOffset);
            submeshes.push_back(submesh);

            vertexOffset += mesh->mNumVertices;
        }

        Ref<Mesh> newMesh = createRef<Mesh>(node->mName.C_Str());
        newMesh->SetData(vertices, indices);
        for (const auto& sm : submeshes) {
            newMesh->AddSubmesh(sm);
        }
        return newMesh;
	}

	void SharedModelAssetData::Extract(const std::string& path) {

	}
}

