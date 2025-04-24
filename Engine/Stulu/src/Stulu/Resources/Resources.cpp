#include "st_pch.h"
#include "Resources.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Renderer/Material/TestMaterial.h"
#include "Assets/ShaderAsset.h"
#include "Assets/MaterialAsset.h"

namespace Stulu {
	std::string Resources::AppDataDir = "";
	std::string Resources::AppAssetDir = "";
	std::string Resources::EngineDataDir = "Data/";

	void Resources::load() {
		PBRShader();
		SkyBoxShader();

		WhiteTexture();
		BlackTexture();
		LoadingTexture();
		LogoTexture();
		DefaultSkyBox();

		DefaultMaterial();
		ReflectiveMaterial();

		CubeMesh();
		PlaneMesh();
	}

	Texture2D* Resources::BlackTexture() {
		static Texture2DAsset tex;
		if (!tex.IsValid()) {
			Ref<Texture2D> blackTexture = Texture2D::create(1, 1);
			static uint32_t datablack = 0x00000000;
			blackTexture->setData(&datablack, sizeof(uint32_t));

			SharedTexture2DAssetData* asset = new SharedTexture2DAssetData(UUIDBlackTexture, blackTexture);
			if (!AssetsManager::GlobalInstance().Contains(UUIDBlackTexture)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDBlackTexture, true);
			}
			tex = AssetsManager::GlobalInstance().GetAsset<Texture2DAsset>(UUIDBlackTexture);
		}

		return *tex;
	}
	Texture2D* Resources::WhiteTexture() {
		static Texture2DAsset tex;
		if (!tex.IsLoaded()) {
			Ref<Texture2D> whiteTexture = Texture2D::create(1, 1);
			static uint32_t dataWhite = 0xffffffff;
			whiteTexture->setData(&dataWhite, sizeof(uint32_t));

			SharedTexture2DAssetData* asset = new SharedTexture2DAssetData(UUIDWhiteTexture, whiteTexture);
			if (!AssetsManager::GlobalInstance().Contains(UUIDWhiteTexture)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDWhiteTexture, true);
			}
			tex = AssetsManager::GlobalInstance().GetAsset<Texture2DAsset>(UUIDWhiteTexture);
		}
		return *tex;
	}
	Texture2D* Resources::LoadingTexture() {
		static Texture2DAsset tex;
		if (!tex.IsLoaded()) {
			const std::string path = EngineDataDir + "/Stulu/Textures/Loading.png";
			SharedTexture2DAssetData* asset = new SharedTexture2DAssetData(UUIDLoadingTexture, path);
			if (!AssetsManager::GlobalInstance().Contains(UUIDLoadingTexture)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDLoadingTexture, true);
			}
			tex = AssetsManager::GlobalInstance().GetAsset<Texture2DAsset>(UUIDLoadingTexture);
		}
		return *tex;

	}
	Texture2D* Resources::LogoTexture() {
		static Texture2DAsset tex;
		if (!tex.IsLoaded()) {
			const std::string path = EngineDataDir + "/Stulu/Textures/PNG - Logo - White.png";
			SharedTexture2DAssetData* asset = new SharedTexture2DAssetData(UUIDLogoTexture, path);
			if (!AssetsManager::GlobalInstance().Contains(UUIDLogoTexture)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDLogoTexture, true);
			}
			tex = AssetsManager::GlobalInstance().GetAsset<Texture2DAsset>(UUIDLogoTexture);
		}
		return *tex;
	}

	SkyBoxAsset Resources::DefaultSkyBoxAsset() {
		static SkyBoxAsset tex;
		if (!tex.IsLoaded()) {
			const std::string path = EngineDataDir + "/Stulu/SkyBox/Default.hdr";
			SharedSkyBoxAssetData* asset = new SharedSkyBoxAssetData(UUIDDefaultSkyBox, path);
			if (!AssetsManager::GlobalInstance().Contains(UUIDDefaultSkyBox)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDDefaultSkyBox, true);
			}
			tex = AssetsManager::GlobalInstance().GetAsset<SkyBoxAsset>(UUIDDefaultSkyBox);
		}
		return tex;
	}

	MeshAsset Resources::CubeMesh() {
		static MeshAsset meshAsset;
		if (!meshAsset.IsLoaded()) {
			static std::vector<Vertex> vertices{
				//top
				{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
				{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
				{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
				{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
				//bottom														 		 
				{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
				{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
				{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
				//right															 		 
				{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
				{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
				{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
				{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
				//left															 		 
				{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
				{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
				{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
				//front															 		 
				{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
				{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},
				{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},
				{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},
				//back															 		 
				{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
				{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
				{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
			};
			static std::vector<uint32_t> indices{
				//top
				2,1,0,
				0,3,2,
				//bottom
				4,5,6,
				6,7,4,
				//right
				10,9,8,
				8,11,10,
				//left
				12,13,14,
				14,15,12,
				//front
				16,17,18,
				18,19,16,
				//back
				22,21,20,
				20,23,22
			};

			Ref<Mesh> mesh = createRef<Mesh>("Cube");
			mesh->SetData(vertices, indices);
			SharedMeshAssetData* asset = new SharedMeshAssetData(UUIDCubeMesh, mesh);

			if (!AssetsManager::GlobalInstance().Contains(UUIDCubeMesh)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDCubeMesh, true);
			}
			meshAsset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(UUIDCubeMesh);
		}
		return meshAsset;
	}
	MeshAsset Resources::PlaneMesh() {
		static MeshAsset meshAsset;
		if (!meshAsset.IsLoaded()) {
			static std::vector<Vertex> vertices{
			{glm::vec3(-0.5f,  0.0f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			{glm::vec3(-0.5f,  0.0f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.0f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f,  0.0f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			};
			static std::vector<uint32_t> indices{
				2,1,0,
				0,3,2,
			};

			Ref<Mesh> mesh = createRef<Mesh>("Plane");
			mesh->SetData(vertices, indices);
			SharedMeshAssetData* asset = new SharedMeshAssetData(UUIDCubeMesh, mesh);

			if (!AssetsManager::GlobalInstance().Contains(UUIDCubeMesh)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDCubeMesh, true);
			}
			meshAsset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(UUIDCubeMesh);
		}
		return meshAsset;
	}

	Ref<VertexArray> Resources::getFullscreenVA() {
		static Ref<VertexArray> m_quadVertexArray;
		if (!m_quadVertexArray) {
			Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
			Stulu::Ref<Stulu::IndexBuffer> indexBuffer;
			m_quadVertexArray = Stulu::VertexArray::create();
			const float vertices[20]{
				-1.0f,-1.0f,.0f, 0.0f,0.0f,
				1.0f,-1.0f,.0f, 1.0f,0.0f,
				1.0f,1.0f,.0f, 1.0f,1.0f,
				-1.0f,1.0f,.0f, 0.0f,1.0f,
			};
			vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(20 * sizeof(float)), vertices);
			vertexBuffer->setLayout({
		{ Stulu::ShaderDataType::Float3, "a_pos" },
		{ Stulu::ShaderDataType::Float2, "a_texCoord" },
				});
			m_quadVertexArray->addVertexBuffer(vertexBuffer);
			uint32_t indices[6]{
				0,1,2,
				2,3,0
			};
			indexBuffer = Stulu::IndexBuffer::create(6u, indices);
			m_quadVertexArray->setIndexBuffer(indexBuffer);
		}
		return m_quadVertexArray;
	}

	Shader* Resources::FullscreenShader() {
		static ShaderAsset shaderAsset;
		if (!shaderAsset.IsLoaded()) {
			SharedShaderAssetData* asset = new SharedShaderAssetData(UUIDQuadShader, Renderer::getShaderSystem()->GetShader("Renderer/QuadFullScreen"));
			if (!AssetsManager::GlobalInstance().Contains(UUIDQuadShader)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDQuadShader, true);
			}
			shaderAsset = AssetsManager::GlobalInstance().GetAsset<ShaderAsset>(UUIDQuadShader);
		}
		return *shaderAsset;
	}
	Shader* Resources::SkyBoxShader() {
		static ShaderAsset shaderAsset;
		if (!shaderAsset.IsLoaded()) {
			SharedShaderAssetData* asset = new SharedShaderAssetData(UUIDSkyBoxShader, Renderer::getShaderSystem()->GetShader("Renderer/SkyBox"));
			if (!AssetsManager::GlobalInstance().Contains(UUIDSkyBoxShader)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDSkyBoxShader, true);
			}
			shaderAsset = AssetsManager::GlobalInstance().GetAsset<ShaderAsset>(UUIDSkyBoxShader);
		}
		return *shaderAsset;
	}
	Shader* Resources::PBRShader() {
		static ShaderAsset shaderAsset;
		if (!shaderAsset.IsLoaded()) {
			SharedShaderAssetData* asset = new SharedShaderAssetData(UUIDPBRShader, Renderer::getShaderSystem()->GetShader("Default/PBR"));
			if (!AssetsManager::GlobalInstance().Contains(UUIDPBRShader)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDPBRShader, true);
			}
			shaderAsset = AssetsManager::GlobalInstance().GetAsset<ShaderAsset>(UUIDPBRShader);
		}
		return *shaderAsset;
	}

	Ref<TestMaterial> Resources::CreateMaterial(
		const std::string& name, const glm::vec4& albedo, float metallic, float roughness, float ambienOcculission, const glm::vec4& emission, 
		const Texture2DAsset& albedoMap, const Texture2DAsset& metallicMap, const Texture2DAsset& roughnessMap, const Texture2DAsset& aoMap, 
		const Texture2DAsset& emissionMap, const Texture2DAsset& normalMap, const glm::vec2& textureTilling, MaterialTransparencyMode transparencyMode, 
		float alphaCutOff, bool instancing) {

		if (!AssetsManager::GlobalInstance().Contains(UUIDPBRShader))
			return nullptr;

		ShaderAsset pbrShader = AssetsManager::GlobalInstance().GetAsset<ShaderAsset>(UUIDPBRShader);
		if (!pbrShader.IsValid())
			return nullptr;

		Ref<TestMaterial> material = createRef<TestMaterial>(Renderer::getShaderSystem()->GetEntry(pbrShader->getName()));
		material->SetTransparencyMode(transparencyMode);
		material->SetAlphaCutOut(alphaCutOff);
		material->SetInstanced(instancing);

		// Texture Maps
		if (albedoMap.IsValid())
			material->SetAndApplyPropertiy<MaterialSampler2DProperty>("Albedo Map", albedoMap);
		if (metallicMap.IsValid())
			material->SetAndApplyPropertiy<MaterialSampler2DProperty>("Metallic Map", metallicMap);
		if (roughnessMap.IsValid())
			material->SetAndApplyPropertiy<MaterialSampler2DProperty>("Roughness Map", roughnessMap);
		if (normalMap.IsValid())
			material->SetAndApplyPropertiy<MaterialSampler2DProperty>("Normal Map", normalMap);
		if (aoMap.IsValid())
			material->SetAndApplyPropertiy<MaterialSampler2DProperty>("Ambient Occlusion Map", aoMap);
		if (emissionMap.IsValid())
			material->SetAndApplyPropertiy<MaterialSampler2DProperty>("Emission Map", emissionMap);

		// Scalar & Vector properties
		material->SetAndApplyPropertiy<MaterialColorProperty>("Albedo", albedo);
		material->SetAndApplyPropertiy<MaterialFloatProperty>("Metallic", metallic);
		material->SetAndApplyPropertiy<MaterialFloatProperty>("Roughness", roughness);
		material->SetAndApplyPropertiy<MaterialFloatProperty>("Ambient Occlusion", ambienOcculission);
		material->SetAndApplyPropertiy<MaterialColorProperty>("Emission", emission);
		material->SetAndApplyPropertiy<MaterialFloat2Property>("Tilling", textureTilling);

		return material;
	}

	MaterialAsset Resources::DefaultMaterialAsset() {
		static MaterialAsset material;
		if (!material.IsLoaded()) {
			Ref<TestMaterial> defaultMaterial = CreateMaterial("Default");

			SharedMaterialAssetData* asset = new SharedMaterialAssetData(UUIDDefaultMaterial, defaultMaterial);
			if (!AssetsManager::GlobalInstance().Contains(UUIDDefaultMaterial)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDDefaultMaterial, true);
			}
			material = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(UUIDDefaultMaterial);
		}
		return material;
	}
	MaterialAsset Resources::ReflectiveMaterialAsset() {
		static MaterialAsset material;
		if (!material.IsLoaded()) {
			Ref<TestMaterial> defaultMaterial = CreateMaterial("Default Reflective", glm::vec4(1.0f), 1.0f, 0.0f, 1.0f);

			SharedMaterialAssetData* asset = new SharedMaterialAssetData(UUIDReflectiveMaterial, defaultMaterial);
			if (!AssetsManager::GlobalInstance().Contains(UUIDReflectiveMaterial)) {
				AssetsManager::GlobalInstance().AddAsset(asset, UUIDReflectiveMaterial, true);
			}
			material = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(UUIDReflectiveMaterial);
		}
		return material;
	}
}
