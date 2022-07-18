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

		static Ref<Mesh> getCubeMesh();
		static Ref<Mesh> getPlaneMesh();
		static Ref<Mesh> getSphereMesh();
		static Ref<Mesh> getCapsuleMesh();
		static MeshAsset& getCubeMeshAsset();
		static MeshAsset& getPlaneMeshAsset();
		static MeshAsset& getSphereMeshAsset();
		static MeshAsset& getCapsuleMeshAsset();

		static Ref<Material> getDefaultMaterial();
		static Ref<Material> getReflectiveMaterial();

		static Ref<Shader> getSkyBoxShader();
	};
}
