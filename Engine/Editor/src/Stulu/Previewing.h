#pragma once
#include <Stulu.h>

namespace Editor {
	class Preview {
	public:
		Preview(size_t CacheSize = 100);
		~Preview();

		Stulu::Ref<Stulu::Texture2D> GetTexturePreview(Stulu::Texture2DAsset texture);
		Stulu::Ref<Stulu::Texture2D> GetRenderTexturePreview(Stulu::RenderTextureAsset texture);
		Stulu::Ref<Stulu::Texture2D> GetMaterialPreview(Stulu::MaterialAsset material);
		Stulu::Ref<Stulu::Texture2D> GetSkyboxPreview(Stulu::SkyBoxAsset skybox);
		Stulu::Ref<Stulu::Texture2D> GetMeshPreview(Stulu::MeshAsset mesh);

		inline bool IsCached(Stulu::UUID uuid) const { return m_cache.find(uuid) != m_cache.end(); }
		inline Stulu::Ref<Stulu::Texture2D> GetCached(Stulu::UUID uuid) { return m_cache.at(uuid).second; }
		void Cache(Stulu::GeneralAsset asset, Stulu::Ref<Stulu::Texture2D> texture);
	private:
		std::unordered_map<Stulu::UUID, std::pair<Stulu::GeneralAsset, Stulu::Ref<Stulu::Texture2D>>> m_cache;

		Stulu::GameObject m_camera;
		Stulu::GameObject m_renderObject;
		Stulu::GameObject m_light;
		Stulu::Ref<Stulu::Scene> m_scene;

		size_t m_cacheLimit;

		void SetUpScene();
		Stulu::Ref<Stulu::Texture2D> RenderScene();

		void SetupMaterial(Stulu::MaterialAsset material);
		void SetupSkybox(Stulu::SkyBoxAsset skybox);
		void SetupMesh(Stulu::MeshAsset mesh);
		void SceneReset();

		void CamDefault();
	};
}