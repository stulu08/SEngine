#include "st_pch.h"
#ifdef ST_DLL_BUILD
//#define ASSIMP_DLL
#endif
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Model.h"
#include "Stulu/Renderer/Renderer.h"
#include <Stulu/Resources/Resources.h>
namespace Stulu {
    /*
    void Stulu::Model::load(const std::string& path) {
        Assimp::Importer importt;
        const aiScene* scene;
        scene = importt.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_GenUVCoords |
            //aiProcess_SortByPType |
            //aiProcess_RemoveRedundantMaterials |
            //aiProcess_FlipWindingOrder | reverse backface culling
            aiProcess_FindInvalidData |
            //aiProcess_FlipUVs |
            //aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_ImproveCacheLocality |
            aiProcess_OptimizeMeshes
            //aiProcess_OptimizeGraph
            //aiProcess_SplitLargeMeshes 
        );
        
        if (!scene || scene->mFlags || !scene->mRootNode) {
            CORE_ERROR(importt.GetErrorString());
            importt.FreeScene();
            return;
        }
        size_t lastS = path.find_last_of("/\\");
        lastS = lastS == std::string::npos ? 0 : lastS + 1;
        size_t lastD = path.rfind('.');
        scene->mRootNode->mName = path.substr(lastS, lastD == std::string::npos ? path.size() - lastS : lastD - lastS).c_str();

        directory = std::filesystem::path(path).parent_path().string();

        for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
            loadMaterial(scene, i);
        }

        processNode(scene->mRootNode, scene);

        importt.FreeScene();
    }
    
    void Stulu::Model::processNode(aiNode* node, const aiScene* scene, UUID& parent) {
        MeshAsset mesh = { std::string(node->mName.data)};
        if (node->mNumMeshes > 0) {
            Ref<Mesh> m;
            if (node->mNumMeshes == 1) {
                aiMesh* a_mesh = scene->mMeshes[node->mMeshes[0]];
                m = processMesh(a_mesh, scene);
                if (materials.find(a_mesh->mMaterialIndex) != materials.end()) {
                    mesh.materialIDs.push_back(a_mesh->mMaterialIndex);
                    int twosided;// 0 = back, 1 = both
                    scene->mMaterials[a_mesh->mMaterialIndex]->Get(AI_MATKEY_TWOSIDED, twosided);
                    mesh.cullMode = twosided ? CullMode::Back : CullMode::BackAndFront;
                }
            }
            else {
                m = processMultiMesh(node, scene);
                for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                    aiMesh* a_mesh = scene->mMeshes[node->mMeshes[i]];
                    if (materials.find(a_mesh->mMaterialIndex) != materials.end()) {
                        mesh.materialIDs.push_back(a_mesh->mMaterialIndex);
                        int twosided;// 0 = back, 1 = both
                        scene->mMaterials[a_mesh->mMaterialIndex]->Get(AI_MATKEY_TWOSIDED, twosided);
                        mesh.cullMode = twosided ? CullMode::Back : CullMode::BackAndFront;
                    }
                }

                if (mesh.materialIDs.size() >= 0) {
                    uint32_t t = mesh.materialIDs[mesh.materialIDs.size() - 1];
                    mesh.materialIDs.clear();
                    mesh.materialIDs.push_back(t);
                }
                

            }
            mesh.mesh = m;
            mesh.hasMesh = true;
        }
        if (parent != UUID::null)
            mesh.parentMeshAsset = parent;

        aiVector3D a_pos, a_scale;

        aiVector3D a_euler;
        node->mTransformation.Decompose(a_scale, a_euler, a_pos);
        glm::quat rot(glm::vec3(a_euler.x, a_euler.y, a_euler.z));

        
        glm::vec3 pos(a_pos.x,a_pos.y,a_pos.z), sca(a_scale.x, a_scale.y, a_scale.z);

        mesh.transform = Math::createMat4(pos, rot, sca);
        
        meshes.push_back(mesh);
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
           processNode(node->mChildren[i], scene, mesh.uuid);
        }
    }

    bool Model::loadMaterial(const aiScene* scene, uint32_t material) {
        if (scene->mNumMaterials < material)
            return false;
        aiMaterial* aMat = scene->mMaterials[material];
        
        aiString aName;
        aiColor3D albedoColor = aiColor3D(1.0f, 1.0f, 1.0f);
        aiColor3D emissionValue = aiColor3D(0.0, 0.0f, 0.0f);
        float opacity = 1.0f, matallicValue = .0f, roughnessValue =.5f, emiisionStrength = 1.0f;
        aiColor3D roughnessVec3 = aiColor3D(roughnessValue);
        aiColor3D metallicVec3 = aiColor3D(matallicValue);
        aMat->Get(AI_MATKEY_COLOR_DIFFUSE, albedoColor);
        aMat->Get(AI_MATKEY_NAME, aName);
        aMat->Get(AI_MATKEY_OPACITY, opacity);
        aMat->Get(AI_MATKEY_SHININESS_STRENGTH, emiisionStrength);
        aMat->Get(AI_MATKEY_COLOR_EMISSIVE, emissionValue);
        aMat->Get(AI_MATKEY_COLOR_REFLECTIVE, roughnessVec3);
        aMat->Get(AI_MATKEY_COLOR_SPECULAR, metallicVec3);
        std::string name(aName.C_Str());
        if (name.empty())
            name = "Material " + std::to_string(material);

        auto& getTexture = [&](aiTextureType type) -> UUID {
            aiString path;
            aMat->GetTexture(type, 0, &path);
            if (!std::string(path.C_Str()).empty()) {
                UUID temp = AssetsManager::getFromPath(directory + "/" + std::string(path.C_Str()));
                if (AssetsManager::existsAndType(temp, AssetType::Texture2D))
                    return temp;
            }
            return UUID::null;
        };

        UUID albedo = getTexture(aiTextureType_DIFFUSE);
        UUID metallic = getTexture(aiTextureType_AMBIENT);
        UUID roughness = getTexture(aiTextureType_SHININESS);
        UUID ambient = getTexture(aiTextureType_LIGHTMAP);
        UUID normal = getTexture(aiTextureType_HEIGHT);
        UUID emission = getTexture(aiTextureType_EMISSIVE);

        //get Transpareny Mode
        TransparencyMode tMode = TransparencyMode::Opaque;
        if (opacity < .99f) {
            tMode = TransparencyMode::Transparent;
        }
        else {
            if (albedo && AssetsManager::exists(albedo)) {
                Ref<Texture> texture = std::any_cast<Ref<Texture>>(AssetsManager::get(albedo).data);
                if (texture) {
                    TextureFormat format = texture->getSettings().format;
                    static Ref<Texture2D> resultTexture = Texture2D::create(1, 1, TextureSettings(TextureFormat::RGBA));
                    Ref<Shader> shader = Renderer::getShaderSystem()->GetShader("Tools/CheckTransparent");

                    uint32_t data = 0x00000000;
                    shader->bind();
                    if (format == TextureFormat::RGBA) {
                        resultTexture->setData(&data, sizeof(uint32_t));
                        shader->setInt("textureType", 1);
                        shader->setTexture("source1", 1, texture, 0, AccesMode::ReadOnly);
                        shader->setTexture("result", 0, resultTexture, 0, AccesMode::WriteOnly);
                        shader->Dispatch({ texture->getWidth(), texture->getHeight(), 1 }, ComputeUsage::ShaderImageAcces);

                        resultTexture->getData(&data, sizeof(uint32_t));
                        if (data == 0xffffffff)
                            tMode = TransparencyMode::Cutout;
                    }
                    else if (format == TextureFormat::RGBA16F) {
                        resultTexture->setData(&data, sizeof(uint32_t));
                        shader->setInt("textureType", 2);
                        shader->setTexture("source2", 2, texture, 0, AccesMode::ReadOnly);
                        shader->setTexture("result", 0, resultTexture, 0, AccesMode::WriteOnly);
                        shader->Dispatch({ texture->getWidth(), texture->getHeight(), 1 }, ComputeUsage::ShaderImageAcces);

                        resultTexture->getData(&data, sizeof(uint32_t));
                        if (data == 0xffffffff)
                            tMode = TransparencyMode::Cutout;
                    }
                    else if (format == TextureFormat::RGBA32F) {
                        resultTexture->setData(&data, sizeof(uint32_t));
                        shader->setInt("textureType", 3);
                        shader->setTexture("source3", 3, texture, 0, AccesMode::ReadOnly);
                        shader->setTexture("result", 0, resultTexture, 0, AccesMode::WriteOnly);
                        shader->Dispatch({ texture->getWidth(), texture->getHeight(), 1 }, ComputeUsage::ShaderImageAcces);

                        resultTexture->getData(&data, sizeof(uint32_t));
                        if (data == 0xffffffff)
                            tMode = TransparencyMode::Cutout;
                    }
                    texture->getSettings().filtering = TextureFiltering::Trilinear;
                    std::dynamic_pointer_cast<Texture2D>(texture)->updateParameters();
                }
            }
        }

        Ref<Material> mat = Resources::createMaterial(name, UUID(),
            glm::vec4(albedoColor.r, albedoColor.g, albedoColor.b, opacity), matallicValue, roughnessValue, .2f, glm::vec4(emissionValue.r, emissionValue.g, emissionValue.b, emiisionStrength),
            albedo, metallic, roughness, ambient, emission, normal, { 1, 1 }, tMode
            );
        materials[material] = mat;
        return true;
    }
    
    Ref<Mesh> Stulu::Model::processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        {
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
                else
                    vertex.texCoords = glm::vec2(0.0f, 0.0f);
                if (mesh->mColors[0]) {
                    vertex.color = { mesh->mColors[i]->r, mesh->mColors[i]->g, mesh->mColors[i]->b, mesh->mColors[i]->a };
                }

                vertices.push_back(vertex);
            }
        }
        {
            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }
        }
        Ref<Mesh> newMesh = createRef<Mesh>(mesh->mName.C_Str());
        newMesh->SetData(vertices, indices);
        return newMesh;
    }
    Ref<Mesh> Stulu::Model::processMultiMesh(aiNode* node, const aiScene* scene) {
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
    */
}