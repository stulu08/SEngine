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
	void Scene::destroyGameObject(GameObject& gameObject) {
		m_registry.destroy(gameObject);
		gameObject = GameObject::null;
	}
	void Scene::onUpdateEditor(Timestep ts, const SceneCamera& camera) {
		calculateLights();
		//rendering
		if (camera.getCamera()) {
			Renderer2D::beginScene(camera.getCamera(), camera.getTransform()); {
				auto group = m_registry.group<TransformComponent>(entt::get<ModelRendererComponent>);
				for (auto gameObject : group)
				{
					auto [transform, model] = group.get<TransformComponent, ModelRendererComponent>(gameObject);
					model.model.submitToRenderer(model.shader, transform);
				}

				auto view = m_registry.view<TransformComponent, SpriteRendererComponent>();
				for (auto gameObject : view)
				{
					auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(gameObject);
					Renderer2D::drawQuad(transform, sprite.color);
				}
			}
			Renderer2D::endScene();
		}
	}
	void Scene::onUpdateRuntime(Timestep ts) {
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, auto& behaviour) {
			if (!behaviour.instance) {
				behaviour.instance = behaviour.InstantiateBehaviour();
				behaviour.instance->gameObject = GameObject{ gameObject, this };
				behaviour.instance->start();
			}
			behaviour.instance->update(ts);
			});
		//getCamera
		GameObject mainCameraObject = getMainCamera();

		//calculations
		calculateLights();
		//rendering
		if (mainCameraObject) {
			Ref<Camera> mainCamera = mainCameraObject.getComponent<CameraComponent>().cam;
			TransformComponent mainCameraTransform = mainCameraObject.getComponent<TransformComponent>();

			Renderer2D::beginScene(mainCamera, mainCameraTransform); {
				auto group = m_registry.group<TransformComponent>(entt::get<ModelRendererComponent>);
				for (auto gameObject : group)
				{
					auto [transform, model] = group.get<TransformComponent, ModelRendererComponent>(gameObject);
					model.model.submitToRenderer(model.shader, transform);
				}

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

	void Scene::onViewportResize(uint32_t width, uint32_t height) {
		m_viewportWidth = width;
		m_viewportHeight = height;
		auto view = m_registry.view<CameraComponent>();
		for (auto entity : view) {
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.settings.staticAspect)
				cameraComponent.onResize(width, height);
		}
	}

	GameObject Scene::getMainCamera() {
		auto group = m_registry.group<GameObjectBaseComponent, CameraComponent>();
		for (auto gameObject : group) {
			auto base = group.get<GameObjectBaseComponent>(gameObject);
			if (base.tag == "MainCam") {
				return GameObject{ gameObject,this };
			}
		}
		return GameObject::null;
	}

	void Scene::calculateLights() {
		auto view = m_registry.view<TransformComponent, LightComponent>();
		for (auto gameObject : view)
		{
			auto [transform, light] = view.get<TransformComponent, LightComponent>(gameObject);
			if (m_data.lightData.lightCount < MAXLIGHTS) {
				m_data.lightData.lights[m_data.lightData.lightCount].colorAndStrength = glm::vec4(light.color, light.stength);
				m_data.lightData.lights[m_data.lightData.lightCount].positionAndType = glm::vec4(transform.position, light.lightType);
				m_data.lightData.lights[m_data.lightData.lightCount].rotation = glm::vec4(transform.forwardDirection(), 1.0f);
				if (light.lightType == LightComponent::Spot)
					m_data.lightData.lights[m_data.lightData.lightCount].spotLightData = glm::vec4(glm::cos(glm::radians(light.spotLight_cutOff)), glm::cos(glm::radians(light.spotLight_outerCutOff)), 1.0f, 1.0f);
				else
					m_data.lightData.lights[m_data.lightData.lightCount].spotLightData = glm::vec4(1.0f);

				m_data.lightData.lightCount++;
			}
		}

		m_data.lightBuffer->setData(&m_data.lightData, sizeof(SceneData::LightData));
		m_data.lightData = SceneData::LightData();
	}

	template<typename T>
	void Scene::onComponentAdded(GameObject gameObject, T& component) {  }
	template<>
	void Scene::onComponentAdded<GameObjectBaseComponent>(GameObject gameObject, GameObjectBaseComponent& component) { }
	template<>
	void Scene::onComponentAdded<TransformComponent>(GameObject gameObject, TransformComponent& component) { }
	template<>
	void Scene::onComponentAdded<CameraComponent>(GameObject gameObject, CameraComponent& component) { if (m_viewportWidth > 0 && m_viewportHeight > 0)component.onResize(m_viewportWidth, m_viewportHeight); }
	template<>
	void Scene::onComponentAdded<LightComponent>(GameObject gameObject, LightComponent& component) { }
	template<>
	void Scene::onComponentAdded<ModelRendererComponent>(GameObject gameObject, ModelRendererComponent& component) { }
	template<>
	void Scene::onComponentAdded<SpriteRendererComponent>(GameObject gameObject, SpriteRendererComponent& component) { }
	template<>
	void Scene::onComponentAdded<NativeBehaviourComponent>(GameObject gameObject, NativeBehaviourComponent& component) { }
}
