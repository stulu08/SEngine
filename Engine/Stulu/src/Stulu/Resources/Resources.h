#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Scene/Model.h"
#include "Stulu/Renderer/Texture.h"
#include "Stulu/Renderer/Material/TestMaterial.h"
#include "Stulu/Resources/Assets/MeshAsset.h"
#include "Stulu/Resources/Assets/TextureAssets.h"
#include "Stulu/Resources/Assets/MaterialAsset.h"

namespace Stulu {
	class STULU_API Material;
	class STULU_API Resources {
	public:
		static void load();
		static void ReleaseAll();

		static Texture2D* BlackTexture();
		static Texture2D* WhiteTexture();
		static Texture2D* LoadingTexture();
		static Texture2D* LogoTexture();

		static SkyBoxAsset DefaultSkyBoxAsset();
		static inline SkyBox* DefaultSkyBox() { return *DefaultSkyBoxAsset(); }

		static MeshAsset CubeMesh();
		static MeshAsset PlaneMesh();
		//static MeshAsset SphereMesh();
		//static MeshAsset HighResSphereMesh();
		//static MeshAsset IcoSphereMesh();
		//static MeshAsset CapsuleMesh();

		static Ref<VertexArray> getFullscreenVA();
		static Shader* FullscreenShader();
		static Shader* SkyBoxShader();
		static Shader* PBRShader();

		static MaterialAsset DefaultMaterialAsset();
		static MaterialAsset ReflectiveMaterialAsset();
		static inline TestMaterial* DefaultMaterial() { return *DefaultMaterialAsset(); }
		static inline TestMaterial* ReflectiveMaterial() { return *ReflectiveMaterialAsset(); }


		//Creates a material based of the default shader
		static Ref<TestMaterial> CreateMaterial(
			const std::string& name,
			const glm::vec4& albedo = { .9f,.9f,.9f,1.0f }, 
			float metallic = 0.0f, 
			float roughness = 0.5f, 
			float ambienOcculission = .2f, 
			const glm::vec4& emission = { 1.0f,1.0f ,1.0f ,0.0f },
			const Texture2DAsset& albedoMap = nullptr,
			const Texture2DAsset& metallicMap = nullptr,
			const Texture2DAsset& roughnessMap = nullptr,
			const Texture2DAsset& aoMap = nullptr,
			const Texture2DAsset& emissionMap = nullptr,
			const Texture2DAsset& normalMap = nullptr,
			const glm::vec2& textureTilling = { 1,1 }, 
			MaterialTransparencyMode transparencyMode = MaterialTransparencyMode::Opaque, 
			float alphaCutOff = .5f,
			bool instancing = false
		);


		// the asset directory of of the Project
		static std::string AppDataDir;
		static std::string AppAssetDir;
		static std::string EngineDataDir;

		static inline const UUID UUIDSkyBoxShader = 7;
		static inline const UUID UUIDQuadShader = 8;
		static inline const UUID UUIDPBRShader = 9;
		static inline const UUID UUIDTerrainShader = 10;
		
		static inline const UUID UUIDDefaultSkyBox = 11;
		static inline const UUID UUIDDefaultMaterial = 12;
		static inline const UUID UUIDReflectiveMaterial = 13;
		static inline const UUID UUIDTerrainMaterial = 14;
		
		static inline const UUID UUIDBlackTexture = 301;
		static inline const UUID UUIDWhiteTexture = 302;
		static inline const UUID UUIDLoadingTexture = 303;
		static inline const UUID UUIDLogoTexture = 304;
		
		static inline const UUID UUIDCubeMesh = 401;
		static inline const UUID UUIDPlaneMesh = 402;
		static inline const UUID UUIDSphereMesh = 404;
		static inline const UUID UUIDCapsuleMesh = 405;
		static inline const UUID UUIDIIcoSphereMesh = 406;
		static inline const UUID UUIDIHighResSphereMesh = 407;

		static inline MeshAsset SphereMesh() {
			return CubeMesh();
		}
		static inline MeshAsset CapsuleMesh() {
			return CubeMesh();
		}
	};
}
