#include "st_pch.h"
#ifdef ST_DLL_BUILD
//#define ASSIMP_DLL
#endif
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Model.h"
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
    SubMesh Stulu::Model::processSubMesh(aiMesh* mesh, const aiScene* scene) {
        ST_PROFILING_FUNCTION();
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        {
            ST_PROFILING_SCOPE("Vertices loading - Stulu::Model::processMesh(aiMesh*, const aiScene*)");
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
                    vertex.normal = vector;
                }
                if (mesh->mTextureCoords[0]) {
                    glm::vec2 vec;
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.texCoords = vec;
                }
                else
                    vertex.texCoords = glm::vec2(0.0f, 0.0f);

                vertices.push_back(vertex);
            }
        }
        {
            ST_PROFILING_SCOPE("Indices loading - Stulu::Model::processMesh(aiMesh*, const aiScene*)");
            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }
        }
        return SubMesh(vertices, indices);
    }
    
    void Model::processMaterials(const aiScene* scene) {
        
    }

    void Stulu::Model::load(const std::string& path) {
        ST_PROFILING_FUNCTION();

        Assimp::Importer importt;
        const aiScene* scene;
        {
            ST_PROFILING_SCOPE("reading file - Stulu::Model::load(const std::string&)");
            scene = importt.ReadFile(path, aiProcess_Triangulate);
        }
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            CORE_ASSERT(false,importt.GetErrorString());
            return;
        }
        size_t lastS = path.find_last_of("/\\");
        lastS = lastS == std::string::npos ? 0 : lastS + 1;
        size_t lastD = path.rfind('.');
        scene->mRootNode->mName = path.substr(lastS, lastD == std::string::npos ? path.size() - lastS : lastD - lastS).c_str();
        processNode(scene->mRootNode, scene);
        processMaterials(scene);
    }
    void Stulu::Model::processNode(aiNode* node, const aiScene* scene, UUID& parent) {
        ST_PROFILING_FUNCTION();
        MeshAsset mesh = { std::string(node->mName.data)};
        if (node->mNumMeshes > 0) {
            Mesh m;
            if (node->mNumMeshes == 1) {
                aiMesh* a_mesh = scene->mMeshes[node->mMeshes[0]];
                m = processMesh(a_mesh, scene);
            }
            else {
                for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                    aiMesh* a_mesh = scene->mMeshes[node->mMeshes[i]];
                    m.addSubMesh(processSubMesh(a_mesh, scene));
                }
                m = Mesh::combine(m);

            }
            mesh.mesh = createRef<Mesh>(m);
            mesh.hasMesh = true;
        }
        if (parent != UUID::null)
            mesh.parentMeshAsset = parent;

        //should work but it doesn't work
        //mesh.transform = *(glm::mat4*)&node->mTransformation;
        aiVector3D a_pos, a_scale;
        aiQuaternion a_rot;
        node->mTransformation.Decompose(a_scale, a_rot, a_pos);
        glm::vec3 pos(a_pos.x,a_pos.y,a_pos.z), sca(a_scale.x, a_scale.y, a_scale.z), rot;
        Math::decomposeTransform(Math::createMat4(pos, glm::quat(a_rot.w, a_rot.x, a_rot.y, a_rot.z), sca), pos, rot, sca);
        mesh.transform = Math::createMat4(pos, glm::degrees(rot), sca);
        
        meshes.push_back(mesh);
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
           processNode(node->mChildren[i], scene, mesh.uuid);
        }
    }
    Mesh Stulu::Model::processMesh(aiMesh* mesh, const aiScene* scene) {
        ST_PROFILING_FUNCTION();
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        {
            ST_PROFILING_SCOPE("Vertices loading - Stulu::Model::processMesh(aiMesh*, const aiScene*)");
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
                    vertex.normal = vector;
                }
                if (mesh->mTextureCoords[0]) {
                    glm::vec2 vec;
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.texCoords = vec;
                }
                else
                    vertex.texCoords = glm::vec2(0.0f, 0.0f);

                vertices.push_back(vertex);
            }
        }
        {
            ST_PROFILING_SCOPE("Indices loading - Stulu::Model::processMesh(aiMesh*, const aiScene*)");
            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }
        }
        return Mesh(vertices, indices);
    }
}