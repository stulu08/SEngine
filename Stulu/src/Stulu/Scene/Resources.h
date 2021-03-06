#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/Texture.h"
#include "Stulu/Scene/Material.h"
namespace Stulu {
	class STULU_API Resources {
	public:
		static void load();

		static Ref<Mesh>& getCubeMesh();
		static Ref<Mesh>& getPlaneMesh();
		static Ref<Mesh>& getSphereMesh();
		static Ref<Mesh>& getSkyBoxMesh();
		static MeshAsset& getCubeMeshAsset();
		static MeshAsset& getPlaneMeshAsset();
		static MeshAsset& getSphereMeshAsset();
		static MeshAsset& getSkyBoxMeshAsset();
		static Ref<Texture2D>& getBlackTexture();
		static Ref<Texture2D>& getWhiteTexture();
		static Ref<Shader>& getSkyBoxShader();
		static Material* getReflectiveMaterial();
		static Material* getDefaultMaterial();
	private:
		static void loadCubeMesh();
		static void loadPlaneMesh();
		static void loadSphereMesh();
		static void loadSkyBoxMesh();
		static void loadBlackTexture();
		static void loadWhiteTexture();

		static void loadSkyBoxShader();

		static void loadReflectiveMaterial();
		static void loadDefaultMaterial();
	};
}
