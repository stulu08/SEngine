#include "Previewing.h"
#include <Stulu/Scene/Components/Components.h>
#include <Stulu/Scene/SceneRenderer.h>
#include <Stulu/Resources/Resources.h>

using namespace Stulu;

namespace Editor {
	
	Preview::Preview(size_t CacheSize) 
		: m_cacheLimit(CacheSize){
		m_cache.reserve(m_cacheLimit);

		SetUpScene();
	}

	Preview::~Preview() {
		m_cache.clear();
		m_scene.reset();
	}

	Stulu::Ref<Stulu::Texture2D> Preview::GetTexturePreview(Stulu::Texture2DAsset textureAsset) {
		if (IsCached(textureAsset.GetUUID())) {
			return GetCached(textureAsset.GetUUID());
		}
		Ref<Texture2D> texture = textureAsset.GetAsset()->GetTextureHandle();
		Cache(GeneralAsset(textureAsset.GetAsset()), texture);
		return texture;
	}

	Stulu::Ref<Stulu::Texture2D> Preview::GetRenderTexturePreview(Stulu::RenderTextureAsset textureAsset) {
		if (IsCached(textureAsset.GetUUID())) {
			return GetCached(textureAsset.GetUUID());
		}
		Ref<Texture2D> texture = textureAsset.GetAsset()->GetTextureHandle();
		Cache(GeneralAsset(textureAsset.GetAsset()), texture);
		return texture;
	}

	Ref<Texture2D> Preview::GetMaterialPreview(MaterialAsset material) {
		if (IsCached(material.GetUUID())) {
			return GetCached(material.GetUUID());
		}
		SetupMaterial(material);
		auto texture = RenderScene();
		Cache(GeneralAsset(material.GetAsset()), texture);
		SceneReset();

		return texture;
	}

	Ref<Texture2D> Preview::GetSkyboxPreview(SkyBoxAsset skybox) {
		if (IsCached(skybox.GetUUID())) {
			return GetCached(skybox.GetUUID());
		}

		SetupSkybox(skybox);
		auto texture = RenderScene();
		Cache(GeneralAsset(skybox.GetAsset()), texture);
		SceneReset();

		return texture;
	}

	Ref<Texture2D> Preview::GetMeshPreview(MeshAsset mesh) {
		if (IsCached(mesh.GetUUID())) {
			return GetCached(mesh.GetUUID());
		}

		SetupMesh(mesh);
		auto texture = RenderScene();
		Cache(GeneralAsset(mesh.GetAsset()), texture);
		SceneReset();

		return texture;
	}

	void Preview::Cache(GeneralAsset asset, Ref<Texture2D> texture) {
		if (m_cache.size() >= m_cacheLimit) {
			m_cache.erase(m_cache.begin());
		}
		m_cache.insert({ asset.GetUUID(), {asset, texture} });
	}

	void Preview::SetUpScene() {
		m_scene = createRef<Scene>();

		m_scene->onViewportResize(200, 200);

		m_camera = m_scene->Create("Camera");
		m_light = m_scene->Create("Light");
		m_light.addComponent<LightComponent>(LightComponent::Directional).strength = 4;
		m_light.getComponent<TransformComponent>().SetRotation(glm::quat(glm::radians(glm::vec3(-45.0f, -45.0f, .0f))));

		CamDefault();
	}

	Ref<Texture2D> Preview::RenderScene() {
		m_scene->getData().enablePhsyics3D = false;
		m_scene->onRuntimeStart();
		m_scene->onUpdateRuntime(Timestep(.05f));
		m_scene->onRuntimeStop();

		m_scene->getRenderer()->ApplyPostProcessing(m_camera.getComponent<CameraComponent>().GetNativeCamera().getFrameBuffer());
		return m_camera.getComponent<CameraComponent>().GetNativeCamera().getFrameBuffer()->getColorAttachment();
	}

	void Preview::SetupMaterial(MaterialAsset material) {
		m_camera.addComponent<SkyBoxComponent>().texture = Resources::DefaultSkyBoxAsset();
		m_camera.getComponent<CameraComponent>().SetClearType(ClearType::Skybox);
		m_renderObject = m_scene->Create("RenderObject");
		m_renderObject.saveAddComponent<MeshFilterComponent>().SetMesh(Resources::CubeMesh());
		m_renderObject.saveAddComponent<MeshRendererComponent>().material = material;
	}
	void Preview::SetupSkybox(SkyBoxAsset skybox) {
		m_camera.addComponent<SkyBoxComponent>().texture = skybox;
		m_camera.getComponent<CameraComponent>().SetClearType(ClearType::Skybox);
		m_renderObject = m_scene->Create("RenderObject");
		m_renderObject.saveAddComponent<MeshFilterComponent>().SetMesh(Resources::CubeMesh());
		m_renderObject.saveAddComponent<MeshRendererComponent>().material = Resources::ReflectiveMaterialAsset();
	}
	void Preview::SetupMesh(MeshAsset mesh) {
		m_camera.addComponent<SkyBoxComponent>().texture = Resources::DefaultSkyBoxAsset();
		m_camera.getComponent<CameraComponent>().SetClearType(ClearType::Skybox);

		if (!mesh.IsLoaded())
			return;

		m_renderObject = m_scene->Create("RenderObject");
		m_renderObject.saveAddComponent<MeshFilterComponent>().SetMesh(mesh);
		m_renderObject.saveAddComponent<MeshRendererComponent>().material = Resources::DefaultMaterialAsset();

		glm::vec3 furthest = glm::abs(mesh->GetBoundingBox().getExtents());

		const float zoom = 2.0f;
		m_camera.getComponent<CameraComponent>().SetFar(std::max({ glm::abs(furthest.x), glm::abs(furthest.y), glm::abs(furthest.z) }) * 2.0f * zoom);
		m_camera.getComponent<CameraComponent>().UpdateCamera();

		auto& transform = m_camera.getComponent<TransformComponent>();
		transform.SetPosition(glm::abs(glm::vec3(0.0f, furthest.y, furthest.z) * zoom));
		transform.SetRotation(Math::lookAt({ 0,0,0 }, transform.position));
	}

	void Preview::SceneReset() {
		if (m_camera.hasComponent<SkyBoxComponent>()) {
			m_camera.removeComponent<SkyBoxComponent>();
			m_camera.getComponent<CameraComponent>().SetClearType(ClearType::Color);
		}

		CamDefault();

		if(m_renderObject)
			m_scene->Destroy(m_renderObject);
	}
	void Preview::CamDefault() {
		if (m_camera.hasComponent<CameraComponent>())
			m_camera.removeComponent<CameraComponent>();

		CameraComponent& cam = m_camera.addComponent<CameraComponent>(CameraMode::Perspective, m_scene->getViewportWidth(), m_scene->getViewportHeight());
		cam.SetClearColor(glm::vec4(.0f));
		cam.SetClearType(ClearType::Color);

		m_camera.getComponent<GameObjectBaseComponent>().tag = "MainCam";
		m_camera.getComponent<TransformComponent>().SetPosition(glm::vec3(0.0f, 0.0f, 1.25f));
		m_camera.getComponent<TransformComponent>().SetRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

	}
}

