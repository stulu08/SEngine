#include "Previewing.h"
#include <Stulu/Scene/Components/Components.h>
#include <Stulu/Scene/SceneRenderer.h>
#include <Stulu/Core/Resources.h>

using namespace Stulu;

namespace Editor {
	
	Preview::Preview(size_t CacheSize) 
		: m_cacheLimit(CacheSize){
		m_cache.reserve(m_cacheLimit);

		SetUpScene();
	}

	Preview::~Preview()
	{
	}

	Ref<Texture> Preview::GetMaterialPreview(Ref<Material> material) {
		if (IsCached(material->getUUID())) {
			return GetCached(material->getUUID());
		}
		SetupMaterial(material);
		auto texture = RenderScene();
		Cache(material->getUUID(), texture);
		SceneReset();

		return texture;
	}

	Ref<Texture> Preview::GetModelPreview(Model& model) {
		// temporary fix, will rework the whole Assets and Models system
		Stulu::UUID uuid = Stulu::UUID::null;
		if (model.getMeshes().size() <= 0)
			uuid = model.getMeshes()[0].uuid;

		if (IsCached(uuid)) {
			return GetCached(uuid);
		}

		SetupModel(model);
		auto texture = RenderScene();
		Cache(uuid, texture);
		SceneReset();

		return texture;
	}

	Ref<Texture> Preview::GetSkyboxPreview(Ref<SkyBox> skybox) {
		if (IsCached(skybox->uuid)) {
			return GetCached(skybox->uuid);
		}

		SetupSkybox(skybox);
		auto texture = RenderScene();
		Cache(skybox->uuid, texture);
		SceneReset();

		return texture;
	}

	Ref<Texture> Preview::GetMeshPreview(MeshAsset& mesh) {
		if (IsCached(mesh.uuid)) {
			return GetCached(mesh.uuid);
		}

		SetupMesh(mesh);
		auto texture = RenderScene();
		Cache(mesh.uuid, texture);
		SceneReset();

		return texture;
	}

	void Preview::Cache(Stulu::UUID uuid, Ref<Texture> texture) {
		if (m_cache.size() >= m_cacheLimit) {
			m_cache.erase(m_cache.begin());
		}
		m_cache.insert({ uuid, texture });
	}

	void Preview::SetUpScene() {
		m_scene = createRef<Scene>();

		m_scene->onViewportResize(200, 200);

		m_camera = m_scene->createGameObject("Camera");
		m_light = m_scene->createGameObject("Light");
		m_light.addComponent<LightComponent>(LightComponent::Directional).strength = 4;
		m_light.getComponent<TransformComponent>().SetRotation(glm::quat(glm::radians(glm::vec3(-45.0f, -45.0f, .0f))));

		CamDefault();
	}

	Ref<Texture> Preview::RenderScene() {
		m_scene->getData().enablePhsyics3D = false;
		m_scene->onRuntimeStart();
		m_scene->onUpdateRuntime(Timestep(.05f));
		m_scene->onRuntimeStop();

		m_scene->getRenderer()->ApplyPostProcessing(m_camera.getComponent<CameraComponent>().GetNativeCamera().getFrameBuffer());
		return m_camera.getComponent<CameraComponent>().GetNativeCamera().getFrameBuffer()->getColorAttachment();
	}

	void Preview::SetupMaterial(Ref<Material> material) {
		m_camera.addComponent<SkyBoxComponent>().texture = Resources::getDefaultSkyBox();
		m_camera.getComponent<CameraComponent>().SetClearType(ClearType::Skybox);
		m_renderObject = m_scene->createGameObject("RenderObject");
		m_renderObject.saveAddComponent<MeshFilterComponent>().mesh = Resources::getCubeMeshAsset();
		m_renderObject.saveAddComponent<MeshRendererComponent>().material = material;
	}
	void Preview::SetupModel(Model& model) {
		m_camera.addComponent<SkyBoxComponent>().texture = Resources::getDefaultSkyBox();
		m_camera.getComponent<CameraComponent>().SetClearType(ClearType::Skybox);

		m_renderObject = m_scene->addModel(model);

		float furthestX = .0f;
		float furthestY = .0f;
		float furthestZ = .0f;
		for (MeshAsset& mesh : model.getMeshes()) {
			if (mesh.mesh) {
				glm::vec3 temp = mesh.mesh->getFurthesteachAxisFromPos(glm::vec3(.0f), 0);
				if (furthestX < temp.x)
					furthestX = temp.x;
				if (furthestY < temp.y)
					furthestY = temp.y;
				if (furthestZ < temp.z)
					furthestZ = temp.z;
			}
		}
		const float zoom = 1.5f;
		m_camera.getComponent<CameraComponent>().SetFar(std::max({ glm::abs(furthestX), glm::abs(furthestY), glm::abs(furthestZ) }) * 2.0f * zoom);
		m_camera.getComponent<CameraComponent>().UpdateCamera();

		auto& transform = m_camera.getComponent<TransformComponent>();
		transform.SetPosition(glm::abs(glm::vec3(furthestX, furthestY, furthestZ)) * zoom);
		transform.SetRotation(Math::lookAt({ 0,0,0 }, transform.position));
	}
	void Preview::SetupSkybox(Ref<SkyBox> skybox) {
		m_camera.addComponent<SkyBoxComponent>().texture = skybox;
		m_camera.getComponent<CameraComponent>().SetClearType(ClearType::Skybox);
		m_renderObject = m_scene->createGameObject("RenderObject");
		m_renderObject.saveAddComponent<MeshFilterComponent>().mesh = Resources::getHighResSphereMeshAsset();
		m_renderObject.saveAddComponent<MeshRendererComponent>().material = Resources::getReflectiveMaterial();
	}
	void Preview::SetupMesh(MeshAsset& mesh) {
		m_camera.addComponent<SkyBoxComponent>().texture = Resources::getDefaultSkyBox();
		m_camera.getComponent<CameraComponent>().SetClearType(ClearType::Skybox);

		if (!mesh.mesh)
			return;

		m_renderObject = m_scene->createGameObject("RenderObject");
		m_renderObject.saveAddComponent<MeshFilterComponent>().mesh = mesh;
		m_renderObject.saveAddComponent<MeshRendererComponent>().material = Resources::getDefaultMaterial();

		glm::vec3 furthest = mesh.mesh->getFurthesteachAxisFromPos(glm::vec3(.0f), 0);

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
			m_scene->destroyGameObject(m_renderObject);
	}
	void Preview::CamDefault() {
		if (m_camera.hasComponent<CameraComponent>())
			m_camera.removeComponent<CameraComponent>();

		CameraComponent& cam = m_camera.addComponent<CameraComponent>(CameraMode::Perspective);
		cam.SetClearColor(glm::vec4(.0f));
		cam.SetClearType(ClearType::Color);

		m_camera.getComponent<GameObjectBaseComponent>().tag = "MainCam";
		m_camera.getComponent<TransformComponent>().SetPosition(glm::vec3(0.0f, 0.0f, 1.25f));
		m_camera.getComponent<TransformComponent>().SetRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

	}
}

