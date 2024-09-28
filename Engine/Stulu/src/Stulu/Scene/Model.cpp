#include "st_pch.h"
#ifdef ST_DLL_BUILD
//#define ASSIMP_DLL
#endif
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Model.h"
#include "Stulu/Renderer/Renderer.h"
#include <Stulu/Scene/Material.h>
#include <Stulu/Scene/AssetsManager.h>
#include <Stulu/Core/Resources.h>
namespace Stulu {
    void Stulu::Model::load(const std::string& path) {
        ST_PROFILING_FUNCTION();

        Assimp::Importer importt;
        const aiScene* scene;
        {
            ST_PROFILING_SCOPE("reading file - Stulu::Model::load(const std::string&)");
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
        }
        
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
        ST_PROFILING_FUNCTION();
        MeshAsset mesh = { std::string(node->mName.data)};
        if (node->mNumMeshes > 0) {
            Mesh m;
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
                for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                    aiMesh* a_mesh = scene->mMeshes[node->mMeshes[i]];
                    m.addSubMesh(processSubMesh(a_mesh, scene));
                    if (materials.find(a_mesh->mMaterialIndex) != materials.end()) {
                        mesh.materialIDs.push_back(a_mesh->mMaterialIndex);
                        int twosided;// 0 = back, 1 = both
                        scene->mMaterials[a_mesh->mMaterialIndex]->Get(AI_MATKEY_TWOSIDED, twosided);
                        mesh.cullMode = twosided ? CullMode::Back : CullMode::BackAndFront;
                    }
                }
                m = Mesh::combine(m);

                if (mesh.materialIDs.size() >= 0) {
                    uint32_t t = mesh.materialIDs[mesh.materialIDs.size() - 1];
                    mesh.materialIDs.clear();
                    mesh.materialIDs.push_back(t);
                }
                

            }
            mesh.mesh = createRef<Mesh>(m);
            mesh.hasMesh = true;
        }
        if (parent != UUID::null)
            mesh.parentMeshAsset = parent;

        aiVector3D a_pos, a_scale;

        //aiQuaternion a_rot;
        //node->mTransformation.Decompose(a_scale, a_rot, a_pos);
        //glm::quat rot(a_rot.w, a_rot.x, a_rot.y, a_rot.z);

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
        ST_PROFILING_FUNCTION();
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
                    static Ref<Shader> shader;
                    if (!shader) {
                        shader = Renderer::getShaderSystem()->CreateShader("CheckTransparentShader", R"(
#version 430 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba8, binding = 0) uniform writeonly image2D result;

layout(rgba8,   binding = 1) uniform readonly image2D source1;
layout(rgba16f, binding = 2) uniform readonly image2D source2;
layout(rgba32f, binding = 3) uniform readonly image2D source3;

layout(location = 1) uniform int textureType;

void main() {
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

	vec4 color = vec4(1.0);
    if(textureType == 1) {
        color = imageLoad(source1, pixelCoord).rgba;
    }
    else if(textureType == 2) {
        color = imageLoad(source2, pixelCoord).rgba;
    }
    else if(textureType == 3) {
        color = imageLoad(source2, pixelCoord).rgba;
    }

    if(color.a < 0.99)
	    imageStore(result, ivec2(0, 0), vec4(1.0));
}
)");
                    }
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
        /*
        aiString albedoPath;
        aMat->GetTexture(aiTextureType_DIFFUSE, 0, &albedoPath);

        aiString metallicPath;
        aMat->GetTexture(aiTextureType_AMBIENT, 0, &metallicPath);

        aiString normalPath;
        aMat->GetTexture(aiTextureType_HEIGHT, 0, &normalPath);

        aiString roughnessPath;
        aMat->GetTexture(aiTextureType_SHININESS, 0, &roughnessPath);

        aiString ambientPath;
        aMat->GetTexture(aiTextureType_LIGHTMAP, 0, &ambientPath);

        MaterialDataType albedo = { ShaderDataType::Sampler,MaterialTexture{4,nullptr,Resources::getWhiteTexture(), 1,UUID::null},"albedoMap",4 };
        if (!std::string(albedoPath.C_Str()).empty()) {
            UUID albedoUUId = AssetsManager::getFromPath(directory + "/" + std::string(albedoPath.C_Str()));
            if (AssetsManager::existsAndType(albedoUUId, AssetType::Texture2D))
                std::any_cast<MaterialTexture&>(albedo.data).uuid = albedoUUId;
        }

        MaterialDataType metallic = { ShaderDataType::Sampler,MaterialTexture{5,nullptr,Resources::getBlackTexture(),1,UUID::null},"metallicMap",5 };
        if (!std::string(metallicPath.C_Str()).empty()) {
            UUID metallicUUId = AssetsManager::getFromPath(directory + "/" + std::string(metallicPath.C_Str()));
            if (AssetsManager::existsAndType(metallicUUId, AssetType::Texture2D))
                std::any_cast<MaterialTexture&>(metallic.data).uuid = metallicUUId;
        }

        MaterialDataType roughness = { ShaderDataType::Sampler,MaterialTexture{6,nullptr,Resources::getBlackTexture(),1,UUID::null},"roughnessMap",6 };
        if (!std::string(roughnessPath.C_Str()).empty()) {
            UUID roughnessUUId = AssetsManager::getFromPath(directory + "/" + std::string(roughnessPath.C_Str()));
            if (AssetsManager::existsAndType(roughnessUUId, AssetType::Texture2D))
                std::any_cast<MaterialTexture&>(roughness.data).uuid = roughnessUUId;
        }

        MaterialDataType normal = { ShaderDataType::Sampler,MaterialTexture{7,nullptr,Resources::getBlackTexture(),1,UUID::null},"normalMap",7 };
        if (!std::string(normalPath.C_Str()).empty()) {
            UUID normalUUId = AssetsManager::getFromPath(directory + "/" + std::string(normalPath.C_Str()));
            if (AssetsManager::existsAndType(normalUUId, AssetType::Texture2D))
                std::any_cast<MaterialTexture&>(normal.data).uuid = normalUUId;
        }

        MaterialDataType ambient = { ShaderDataType::Sampler,MaterialTexture{8,nullptr,Resources::getBlackTexture(),1,UUID::null},"aoMap",8 };
        if (!std::string(ambientPath.C_Str()).empty()) {
            UUID ambientUUId = AssetsManager::getFromPath(directory + "/" + std::string(ambientPath.C_Str()));
            if (AssetsManager::existsAndType(ambientUUId, AssetType::Texture2D))
                std::any_cast<MaterialTexture&>(ambient.data).uuid = ambientUUId;
        }
        Ref<Material> mat = createRef<Material>(AssetsManager::get(UUID(9)),
            (std::vector<MaterialDataType>{
            MaterialDataType{ ShaderDataType::Float4,glm::vec4(albedoColor.r, albedoColor.g, albedoColor.b, opacity),"albedo",0 },
                MaterialDataType{ ShaderDataType::Float,matallicValue,"metallic",1 },
                MaterialDataType{ ShaderDataType::Float,roughnessValue,"roughness",2 },
                MaterialDataType{ ShaderDataType::Float,.1f,"ao",3 },
                albedo,
                metallic,
                roughness,
                normal,
                ambient,
                MaterialDataType{ ShaderDataType::Float2,glm::vec2(1.0f),"textureTilling",9 },
                MaterialDataType{ ShaderDataType::Int,0,"transparencyMode",10 },
                MaterialDataType{ ShaderDataType::Float,.0f,"alphaCutOff",11 },
                MaterialDataType{ ShaderDataType::Int,1,"useGLTFMetallicRoughnessMap",12 },
        }), name);
        */
        materials[material] = mat;
        return true;
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
                if (mesh->mColors[0]) {
                    vertex.color = { mesh->mColors[i]->r, mesh->mColors[i]->g, mesh->mColors[i]->b, mesh->mColors[i]->a };
                }

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
}