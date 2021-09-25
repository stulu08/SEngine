#include "st_pch.h"
#include "Scene.h"

#include "Stulu/Scene/Behavior.h"
#include "Stulu/Math/Math.h"
#include "GameObject.h"

namespace Stulu {
	Scene::Scene() {
		m_data.lightBuffer = UniformBuffer::create(sizeof(SceneData::LightData), 1);
		
	}
	Scene::~Scene() {
		
	}
	GameObject Scene::createGameObject(const std::string& name) {
		GameObject go = { m_registry.create(), this };
		go.addComponent<GameObjectBaseComponent>(!name.empty() ? name : "GameObject");
		go.addComponent<TransformComponent>();
		return go;
	}
	void Scene::onUpdateEditor(Timestep ts, const SceneCamera& camera) {
		Renderer2D::beginScene(camera.getCamera(), camera.getTransform());
		{
			auto group = m_registry.group<TransformComponent>(entt::get<ModelRendererComponent>);
			for (auto gameObject : group)
			{
				auto [transform, model] = group.get<TransformComponent, ModelRendererComponent>(gameObject);
				model.model.submitToRenderer(model.shader, transform);
			}
		}
		{
			auto view = m_registry.view<LightComponent, TransformComponent>();
			for (auto gameObject : view)
			{
				auto [light, transform] = view.get<LightComponent, TransformComponent>(gameObject);
				if (m_data.lightData.lightCount < MAXLIGHTS) {
					m_data.lightData.lights[m_data.lightData.lightCount].colorAndStrength = glm::vec4(light.color, light.stength);
					m_data.lightData.lights[m_data.lightData.lightCount].positionAndType = glm::vec4(transform.position, light.lightType);
					m_data.lightData.lights[m_data.lightData.lightCount].rotation = glm::vec4(transform.forwardDirection(),1.0f);
					m_data.lightData.lightCount++;
				}
			}

			m_data.lightBuffer->setData(&m_data.lightData, sizeof(SceneData::LightData));
			m_data.lightData = SceneData::LightData();
		}
		{
			auto view = m_registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto gameObject : view)
			{
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(gameObject);
				Renderer2D::drawQuad(transform, sprite.color);
			}
		}
		Renderer2D::endScene();
	}
	void Scene::onUpdateRuntime(Timestep ts){
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, auto& behaviour) {
			if (!behaviour.instance) {
					behaviour.InstantiateFn();
					behaviour.instance->gameObject = GameObject{ gameObject, this};
					behaviour.startFn(behaviour.instance);
			}
			behaviour.updateFn(behaviour.instance, ts);
		});
		{
			//getCamera
			Ref<Camera> mainCamera = nullptr;
			TransformComponent cameraTransform;
			auto view = m_registry.view<CameraComponent>();
			for (auto gameObject : view)
			{
				auto cam = view.get<CameraComponent>(gameObject);
				if (mainCamera == nullptr) {
					mainCamera = cam.cam;
					cameraTransform = m_registry.get<TransformComponent>(gameObject);
				}
			}
			//rendering
			if (mainCamera) {
				Renderer2D::beginScene(mainCamera, cameraTransform);
				{
					auto group = m_registry.group<TransformComponent>(entt::get<ModelRendererComponent>);
					for (auto gameObject : group)
					{
						auto [transform, model] = group.get<TransformComponent, ModelRendererComponent>(gameObject);
						model.model.submitToRenderer(model.shader, transform);
					}
				}
				{
					auto view = m_registry.view<TransformComponent, LightComponent>();
					for (auto gameObject : view)
					{
						auto [transform, light] = view.get<TransformComponent, LightComponent>(gameObject);
						if (m_data.lightData.lightCount < MAXLIGHTS) {
							m_data.lightData.lights[m_data.lightData.lightCount].colorAndStrength = glm::vec4(light.color, light.stength);
							m_data.lightData.lights[m_data.lightData.lightCount].positionAndType = glm::vec4(transform.position, light.lightType);
							m_data.lightData.lights[m_data.lightData.lightCount].rotation = glm::vec4(transform.forwardDirection(), 1.0f);
							m_data.lightData.lightCount++;
						}
					}

					m_data.lightBuffer->setData(&m_data.lightData, sizeof(SceneData::LightData));
					m_data.lightData = SceneData::LightData();
				}
				{
					auto view = m_registry.view<TransformComponent, SpriteRendererComponent>();
					for (auto gameObject : view)
					{
						auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(gameObject);
						Renderer2D::drawQuad(transform, sprite.color);
					}
				}
				Renderer2D::endScene();
			}//no camera
			else {
				ST_WARN("No Camera to Render on");
			}
		}
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height) {
		m_viewportWidth = width;
		m_viewportHeight = height;
		auto view = m_registry.view<CameraComponent>();
		for (auto entity : view) {
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.settings.staticAspect)
				cameraComponent.onResize((float)width, (float)height);
		}
	}

	template<typename T>
	void Scene::onComponentAdded(GameObject gameObject, T& component) { static_assert(false); }
	template<>
	void Scene::onComponentAdded<GameObjectBaseComponent>(GameObject gameObject, GameObjectBaseComponent& component) { }
	template<>
	void Scene::onComponentAdded<TransformComponent>(GameObject gameObject, TransformComponent& component) { }
	template<>
	void Scene::onComponentAdded<CameraComponent>(GameObject gameObject, CameraComponent& component) { if (m_viewportWidth > 0 && m_viewportHeight > 0)component.onResize((float)m_viewportWidth, (float)m_viewportHeight); }
	template<>
	void Scene::onComponentAdded<LightComponent>(GameObject gameObject, LightComponent& component) { }
	template<>
	void Scene::onComponentAdded<ModelRendererComponent>(GameObject gameObject, ModelRendererComponent& component) { }
	template<>
	void Scene::onComponentAdded<SpriteRendererComponent>(GameObject gameObject, SpriteRendererComponent& component) { }
	template<>
	void Scene::onComponentAdded<NativeBehaviourComponent>(GameObject gameObject, NativeBehaviourComponent& component) { }
}
