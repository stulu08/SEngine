#include "st_pch.h"
#include "ModelAsset.h"

#ifdef ST_DLL_BUILD
//#define ASSIMP_DLL
#endif
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/GltfMaterial.h>
#include "TextureAssets.h"
#include <Stulu/Renderer/Material/TestMaterial.h>
#include <Stulu/Resources/Resources.h>
#include <Stulu/Renderer/Material/MaterialProperty.h>

namespace Stulu {

	bool SharedModelAssetData::Load() {
		Assimp::Importer importer;
		//const aiScene* scene = importer.ReadFile(GetPath(), aiProcessPreset_TargetRealtime_Quality);
		const aiScene* scene = importer.ReadFile(GetPath(),
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			//aiProcess_GenUVCoords |
			aiProcess_TransformUVCoords  |
			//aiProcess_SortByPType |
			aiProcess_RemoveRedundantMaterials |
			//aiProcess_FlipWindingOrder | reverse backface culling
			aiProcess_FindInvalidData |
			//aiProcess_FlipUVs |
			//aiProcess_CalcTangentSpace |
			//aiProcess_GenSmoothNormals |
			//aiProcess_ImproveCacheLocality |
			aiProcess_OptimizeMeshes |
			aiProcess_OptimizeGraph
			//aiProcess_SplitLargeMeshes 
		);
		
		if (!scene || !scene->mRootNode) {
			CORE_ERROR("Error during model import: ", importer.GetErrorString());
			importer.FreeScene();
			return {};
		}
		scene->mRootNode->mName = std::filesystem::path(GetPath()).stem().string();
		LoadMaterialPool(scene, GetPath());

		m_rootNode = ProcessNode(scene->mRootNode, scene);

		importer.FreeScene();
		return true;
	}

	ModelNode SharedModelAssetData::ProcessNode(const aiNode* node, const aiScene* scene) {
		std::string nodeName = node->mName.C_Str();

		aiVector3D aiPos, aiRot, aiScale;
		node->mTransformation.Decompose(aiScale, aiRot, aiPos);

		ModelNode resultNode = {};
		resultNode.name = nodeName;
		resultNode.Position = glm::vec3(aiPos.x, aiPos.y, aiPos.z);
		resultNode.Scale = glm::vec3(aiScale.x, aiScale.y, aiScale.z);
		resultNode.Rotation = Math::EulerToQuaternion(glm::radians(glm::vec3(aiRot.x, aiRot.y, aiRot.z)));

		// load meshes
		if (node->mNumMeshes > 0) {
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
			std::vector<MeshSubmesh> submeshes;
			auto& materials = resultNode.mesh.materials;

			size_t indexOffset = 0;
			for (size_t meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {
				aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIndex]];
				
				MeshSubmesh submesh;
				submesh.name = mesh->mName.C_Str();
				submesh.vertexOffset = static_cast<uint32_t>(vertices.size());
				submesh.indexOffset = static_cast<uint32_t>(indices.size()); // indexOffset in global index buffer

				const size_t vertexStart = vertices.size();
				for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
					Vertex vertex = {};
					vertex.pos.x = mesh->mVertices[i].x;
					vertex.pos.y = mesh->mVertices[i].y;
					vertex.pos.z = mesh->mVertices[i].z;

					if (mesh->HasNormals()) {
						vertex.normal.x = mesh->mNormals[i].x;
						vertex.normal.y = mesh->mNormals[i].y;
						vertex.normal.z = mesh->mNormals[i].z;
						vertex.normal = glm::normalize(vertex.normal);
					}

					if (mesh->HasTextureCoords(0)) {
						vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
						vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
					}
					else {
						vertex.texCoords = glm::vec2(0.0f, 0.0f);
					}
					
					if (mesh->HasVertexColors(0)) {
						vertex.color = { mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a };
					}

					vertices.push_back(vertex);
				}

				for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
					aiFace face = mesh->mFaces[i];
					for (unsigned int j = 0; j < face.mNumIndices; j++) {
						indices.push_back(static_cast<uint32_t>(vertexStart + face.mIndices[j]));
					}
				}

				submesh.indexCount = static_cast<uint32_t>(indices.size() - submesh.indexOffset);
				submeshes.push_back(submesh);

				uint32_t materalIndex = mesh->mMaterialIndex;
				if (materalIndex < m_materialPool.size()) {
					resultNode.mesh.materials.push_back(m_materialPool[materalIndex]);
				}
				
			}

			const UUID meshUUID = BuildSubAssetUUID(nodeName);

			// create new mesh in asset registry
			if (!AssetsManager::GlobalInstance().Contains(meshUUID)) {
				SharedMeshAssetData* meshAssetData = new SharedMeshAssetData(meshUUID, createRef<Mesh>());
				AssetsManager::GlobalInstance().AddAsset(meshAssetData, meshUUID);
			}

			MeshAsset meshAsset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(meshUUID);
			meshAsset->ConstructMesh(vertices, indices);
			meshAsset->SetName(nodeName);
			for (const auto& sm : submeshes) {
				meshAsset->AddSubmesh(sm);
			}
			resultNode.mesh.mesh = meshAsset;

		}

		for (size_t childId = 0; childId < node->mNumChildren; childId++) {
			resultNode.children.push_back(ProcessNode(node->mChildren[childId], scene));
		}

		return resultNode;
	}

	inline static glm::vec4 AiToVec4(const aiColor4D& col) {
		return glm::vec4(col.r, col.g, col.b, col.a);
	}

	void SharedModelAssetData::LoadMaterialPool(const aiScene* scene, const std::string& RootPath) {
		// load all materials
		m_materialPool.clear();
		m_materialPool.reserve(scene->mNumMaterials);

		const auto path = std::filesystem::path(RootPath);

		std::string directory = path.parent_path().string();
		bool isGltf = path.extension() == ".gltf" || path.extension() == ".glb";

		for (size_t materialID = 0; materialID < scene->mNumMaterials; materialID++) {
			const aiMaterial* material = scene->mMaterials[materialID];

			aiString aName;
			aiColor4D albedoColor = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
			aiColor4D emissionColor = aiColor4D(0.0f, 0.0f, 0.0f, 1.0f);
			float metallicValue = -1.0f;
			float roughnessValue = -1.0f;
			float ambienLightValue = 0.2f;

			material->Get(AI_MATKEY_NAME, aName);
			material->Get(AI_MATKEY_BASE_COLOR, albedoColor);
			material->Get(AI_MATKEY_COLOR_EMISSIVE, emissionColor);
			material->Get(AI_MATKEY_EMISSIVE_INTENSITY, emissionColor.a);
			material->Get(AI_MATKEY_METALLIC_FACTOR, metallicValue);
			material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughnessValue);

			// alternative non pbr convertions
			if (metallicValue < 0.0f) {
				metallicValue = 0.0f;
				material->Get(AI_MATKEY_REFLECTIVITY, metallicValue);
			}
			if (roughnessValue < 0.0f) {
				roughnessValue = 0.5f;
				material->Get(AI_MATKEY_SHININESS_STRENGTH, roughnessValue);
			}


			aiString texPath;
			auto LoadMap = [&](aiTextureType type, uint32_t index) -> Texture2DAsset {
				if (material->GetTextureCount(type) > 0 && material->GetTexture(type, index, &texPath) == AI_SUCCESS) {
					std::string fullPath = directory + "/" + texPath.C_Str();

					UUID textureID = UUID::null;
					SharedAssetData* assetData = AssetsManager::GlobalInstance().GetFromPath(fullPath);
					if (!assetData) {
						// not yet loaded
						textureID = AssetsManager::GlobalInstance().LoadFile(fullPath);
					}
					else {
						textureID = assetData->GetUUID();
					}

					return AssetsManager::GlobalInstance().GetAsset<Texture2DAsset>(textureID);
				}
				return nullptr;
			};

			Texture2DAsset albedoMap = LoadMap(AI_MATKEY_BASE_COLOR_TEXTURE);
			Texture2DAsset metallicMap = LoadMap(AI_MATKEY_METALLIC_TEXTURE);
			Texture2DAsset roughnessMap = LoadMap(AI_MATKEY_ROUGHNESS_TEXTURE);
			Texture2DAsset aoMap = LoadMap(aiTextureType_AMBIENT_OCCLUSION, 0);
			Texture2DAsset emissionMap = LoadMap(aiTextureType_EMISSIVE, 0);
			Texture2DAsset normalMap = LoadMap(aiTextureType_NORMALS, 0);
			Texture2DAsset opacityMap = LoadMap(aiTextureType_OPACITY, 0);

			/* 
			Only gltf supports pbr material keys, obj and other types may have diffrent keys
			
			Diffuse -> albedo (kd)
			Ambient -> metallic (ka)
			Specularity/Shininess -> roughness (ks)
			Bump/Height -> normal
			*/
			if (!albedoMap.IsValid()) {
				albedoMap = LoadMap(aiTextureType_DIFFUSE, 0);
			}
			if (!roughnessMap.IsValid()) {
				roughnessMap = LoadMap(aiTextureType_SHININESS, 0);
			}
			if (!roughnessMap.IsValid()) {
				roughnessMap = LoadMap(aiTextureType_SPECULAR, 0);
			}
			if (!aoMap.IsValid()) {
				aoMap = LoadMap(aiTextureType_LIGHTMAP, 0);
			}
			if (!normalMap.IsValid()) {
				normalMap = LoadMap(aiTextureType_HEIGHT, 0);
			}

			float alphaCut = 0.0f;
			MaterialTransparencyMode alphaMode = MaterialTransparencyMode::Opaque;
			if (isGltf) {
				material->Get(AI_MATKEY_GLTF_ALPHACUTOFF, alphaCut);

				aiString aiAlphaMode;
				if (material->Get(AI_MATKEY_GLTF_ALPHAMODE, aiAlphaMode) == AI_SUCCESS) {
					std::string mode = aiAlphaMode.C_Str();
					mode = mode.substr(0, mode.find_first_of('\0'));
					if (mode == "OPAQUE") alphaMode = MaterialTransparencyMode::Opaque;
					else if (mode == "MASK") alphaMode = MaterialTransparencyMode::Cutout;
					else if (mode == "BLEND") alphaMode = MaterialTransparencyMode::Transparent;
				}
			}
			else {
				float opacity = 1.0f;
				material->Get(AI_MATKEY_OPACITY, opacity);
				alphaMode = (opacity < 1.0f) ? MaterialTransparencyMode::Transparent : MaterialTransparencyMode::Opaque;
			}


			auto createdMaterial = Resources::CreateMaterial(
				aName.C_Str(), AiToVec4(albedoColor), metallicValue, roughnessValue,
				ambienLightValue, AiToVec4(emissionColor), albedoMap, metallicMap, roughnessMap,
				aoMap, emissionMap, normalMap, { 1.0f, 1.0f }, alphaMode, alphaCut, true);
			
			if (opacityMap.IsValid()) {
				createdMaterial->SetAndApplyPropertiy<MaterialSampler2DProperty>("Mask Map", opacityMap);
				if (!createdMaterial->IsTransparent())
					createdMaterial->SetTransparencyMode(MaterialTransparencyMode::Cutout);
			}

			UUID materialUUID = BuildSubAssetUUID(createdMaterial->GetName() + std::to_string(materialID));
			// create new material in asset registry
			if (!AssetsManager::GlobalInstance().Contains(materialUUID)) {
				SharedMaterialAssetData* materialAsssetData = new SharedMaterialAssetData(materialUUID, createdMaterial);
				AssetsManager::GlobalInstance().AddAsset(materialAsssetData, materialUUID);
			}
			MaterialAsset materialAsset = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(materialUUID);
			materialAsset.GetAsset()->SetMaterial(createdMaterial);
			m_materialPool.push_back(materialAsset);
		}
	}
}

