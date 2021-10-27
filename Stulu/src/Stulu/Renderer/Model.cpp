#include "st_pch.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Model.h"
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
    GameObject Model::loadModel(const std::string& path, Scene* scene, const Ref<Material>& material) {
        ST_PROFILING_FUNCTION();

        Assimp::Importer importer;
        const aiScene* a_scene;
        {
            ST_PROFILING_SCOPE("reading file - Stulu::Model::load(const std::string&)");
            a_scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
        }

        if (!a_scene || a_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !a_scene->mRootNode) {
            CORE_ASSERT(false, importer.GetErrorString());
            return GameObject::null;
        }
        a_scene->mRootNode->mName = a_scene->GetShortFilename(path.c_str());
        return processNode(a_scene->mRootNode, a_scene, scene, material);;
    }
    GameObject Stulu::Model::processNode(aiNode* node, const aiScene* scene, Scene* s_scene, const Ref<Material>& material) {
        ST_PROFILING_FUNCTION();
        GameObject go = s_scene->createGameObject(std::string(node->mName.data));

        glm::vec3 pos(.0f), rot(.0f), scale(1.0f);
        decompose(node->mTransformation, pos, rot, scale);
        go.getComponent<TransformComponent>().position = pos;
        go.getComponent<TransformComponent>().rotation = rot;
        go.getComponent<TransformComponent>().scale = scale;

        if (node->mNumMeshes > 0) {
            Mesh mesh;
            if (node->mNumMeshes == 1) {
                aiMesh * a_mesh = scene->mMeshes[node->mMeshes[0]];
                mesh = processMesh(a_mesh, scene);
            }
            else {
                for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                    aiMesh* a_mesh = scene->mMeshes[node->mMeshes[i]];
                    mesh.addSubMesh(processSubMesh(a_mesh, scene));
                }
                mesh = Mesh::combine(mesh);
            }
            go.addComponent<MeshRendererComponent>().material = material;
            go.getComponent<MeshFilterComponent>().mesh = createRef<Mesh>(mesh);
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            GameObject g = processNode(node->mChildren[i], scene, s_scene, material);
            go.getComponent<TransformComponent>().addChild(g);
        }
        return go;
    }
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

    const void Model::decompose(const aiMatrix4x4& aMat, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale) {
        aiVector3D a_pos;
        aiVector3D a_scale;
        aiQuaternion a_rot;

        aMat.Decompose(a_scale, a_rot, a_pos);
        glm::vec3 pos, sca;
        pos.x = a_pos.x;
        pos.y = a_pos.y;
        pos.z = a_pos.z;
        sca.x = a_scale.x;
        sca.y = a_scale.y;
        sca.z = a_scale.z;
        glm::quat q = glm::quat(a_rot.w, a_rot.x, a_rot.y, a_rot.z);//q is only pitch yaw roll, but we want it in x y z
        Math::decomposeTransform(Math::createMat4(pos, q, sca), position, rotation, scale);

    }

    void Stulu::Model::load(const std::string& path) {
        ST_PROFILING_FUNCTION();

        Assimp::Importer import;
        const aiScene* scene;
        {
            ST_PROFILING_SCOPE("reading file - Stulu::Model::load(const std::string&)");
            scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        }
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            CORE_ASSERT(false,import.GetErrorString());
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }
    void Stulu::Model::processNode(aiNode* node, const aiScene* scene) {
        ST_PROFILING_FUNCTION();
        if (node->mNumMeshes > 0) {
            Mesh mesh;
            if (node->mNumMeshes == 1) {
                aiMesh* a_mesh = scene->mMeshes[node->mMeshes[0]];
                mesh = processMesh(a_mesh, scene);
            }
            else {
                for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                    aiMesh* a_mesh = scene->mMeshes[node->mMeshes[i]];
                    mesh.addSubMesh(processSubMesh(a_mesh, scene));
                }
            }
            meshes.push_back(mesh);
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
           processNode(node->mChildren[i], scene);
           
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