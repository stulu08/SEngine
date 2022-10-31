#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Scene/Model.h"
#include "Stulu/Renderer/Texture.h"
#include "Stulu/Scene/Material.h"
namespace Stulu {
	class STULU_API Resources {
	public:
		static void load();

		static Ref<Texture> getBlackTexture();
		static Ref<Texture> getWhiteTexture();
		static Ref<Texture> getLoadingTexture();
		static Ref<Texture> getLogoTexture();

		static Ref<SkyBox> getDefaultSkyBox();
		static Asset& getDefaultSkyBoxAsset();

		static Ref<Mesh> getCubeMesh();
		static Ref<Mesh> getPlaneMesh();
		static Ref<Mesh> getSphereMesh();
		static Ref<Mesh> getCapsuleMesh();
		static MeshAsset& getCubeMeshAsset();
		static MeshAsset& getPlaneMeshAsset();
		static MeshAsset& getSphereMeshAsset();
		static MeshAsset& getCapsuleMeshAsset();

		static Ref<VertexArray>& getFullscreenVA();
		static Ref<Shader>& getFullscreenShader();

		static Ref<Material> getDefaultMaterial();
		static Ref<Material> getReflectiveMaterial();
		//Creates a material based of the default shader
		static Ref<Material> createMaterial(const std::string& name, const UUID& uuid = UUID(),
			const glm::vec4& albedo = { .9f,.9f,.9f,1.0f }, const float& metallic = 0.0f, const float& roughness = 0.5f, const float& ao = .2f, const glm::vec4& emission = { 1.0f,1.0f ,1.0f ,0.0f },
			const UUID& albedoMap = UUID::null, const UUID& metallicMap = UUID::null, const UUID& roughnessMap = UUID::null, const UUID& aoMap = UUID::null, const UUID& emissionMap = UUID::null,
			const UUID& normalMap = UUID::null, const glm::vec2& textureTilling = { 1,1 }, TransparencyMode transparencyMode = TransparencyMode::Opaque, float alphaCutOff = .5f
		);

		static Ref<Shader> getSkyBoxShader();
	};
}
