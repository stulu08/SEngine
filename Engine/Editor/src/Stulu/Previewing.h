#pragma once
#include <Stulu/Renderer/Texture.h>
#include <Stulu/Scene/Model.h>
#include <Stulu/Scene/GameObject.h>
#include <Stulu/Scene/AssetsManager.h>
#include <Stulu/Scene/Material.h>

namespace Editor {
	class Preview {
	public:
		Preview(size_t CacheSize = 50);
		~Preview();

		Stulu::Ref<Stulu::Texture> GetMaterialPreview(Stulu::Ref<Stulu::Material> material);
		Stulu::Ref<Stulu::Texture> GetModelPreview(Stulu::Model& model);
		Stulu::Ref<Stulu::Texture> GetSkyboxPreview(Stulu::Ref<Stulu::SkyBox> skybox);
		Stulu::Ref<Stulu::Texture> GetMeshPreview(Stulu::MeshAsset& mesh);

		inline bool IsCached(Stulu::UUID uuid) const { return m_cache.find(uuid) != m_cache.end(); }
		inline Stulu::Ref<Stulu::Texture> GetCached(Stulu::UUID uuid) { return m_cache.at(uuid); }
		void Cache(Stulu::UUID uuid, Stulu::Ref<Stulu::Texture> texture);
	private:
		std::unordered_map<Stulu::UUID, Stulu::Ref<Stulu::Texture>> m_cache;

		Stulu::GameObject m_camera;
		Stulu::GameObject m_renderObject;
		Stulu::GameObject m_light;
		Stulu::Ref<Stulu::Scene> m_scene;

		size_t m_cacheLimit;

		void SetUpScene();
		Stulu::Ref<Stulu::Texture> RenderScene();

		void SetupMaterial(Stulu::Ref<Stulu::Material> material);
		void SetupModel(Stulu::Model& model);
		void SetupSkybox(Stulu::Ref<Stulu::SkyBox> skybox);
		void SetupMesh(Stulu::MeshAsset& mesh);
		void SceneReset();

		void CamDefault();
	};
}