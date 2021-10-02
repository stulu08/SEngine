#include "st_pch.h"
#include "Scene.h"

#include "Stulu/Scene/SceneCamera.h"
#include "Stulu/Scene/Components.h"
#include "Stulu/Scene/Behavior.h"
#include "Stulu/Math/Math.h"
#include "GameObject.h"

namespace Stulu {
	Scene::Scene() {
		ST_PROFILING_FUNCTION();
		m_data.lightBuffer = UniformBuffer::create(sizeof(SceneData::LightData), 1);
		m_shaderLib.load("Stulu/assets/Shaders/pbr.glsl");
	}
	Scene::~Scene() {
		
	}
	GameObject Scene::createGameObject(const std::string& name) {
		ST_PROFILING_FUNCTION();
		GameObject go = { m_registry.create(), this };
		go.addComponent<GameObjectBaseComponent>(!name.empty() ? name : "GameObject");
		go.addComponent<TransformComponent>();
		return go;
	}
	void Scene::destroyGameObject(GameObject gameObject) {
		ST_PROFILING_FUNCTION();
		m_registry.destroy(gameObject);
		gameObject = GameObject::null;
	}

	void Scene::onUpdateEditor(Timestep ts, const SceneCamera& camera) {
		ST_PROFILING_FUNCTION();
		calculateLights();
		if (camera.getCamera()) {
			ST_PROFILING_SCOPE("Scene Camera Rendering");
			ST_PROFILING_RENDERDATA_BEGIN();
			RenderCommand::setClearColor(glm::vec4(glm::vec3(.1f),1.0f));
			Renderer2D::beginScene(camera.getCamera(), camera.getTransform().getTransform());
			RenderCommand::clear();


			{
				auto group = m_registry.view<MeshFilterComponent>();
				for (auto gameObject : group) {
					ST_PROFILING_SCOPE("Rendering Mesh");
					auto& filter = group.get<MeshFilterComponent>(gameObject);
					auto& ren = m_registry.get<MeshRendererComponent>(gameObject);
					glm::mat4 trans = m_registry.get<TransformComponent>(gameObject);

					for (size_t i = 0; i < filter.mesh->getSubMeshCount(); i++) {
						ST_PROFILING_SCOPE("Rendering SubMesh");
						Renderer::submit(filter.mesh->getSubMesh(i).getVertexArray(), ren.shader, trans);

						ST_PROFILING_RENDERDATA_ADDINDICES(filter.mesh->getSubMesh(i).getIndicesCount());
						ST_PROFILING_RENDERDATA_ADDVERTICES(filter.mesh->getSubMesh(i).getVerticesCount());
					}
					if (filter.mesh->getVertexArray() != nullptr) {
						Renderer::submit(filter.mesh->getVertexArray(), ren.shader, trans);

						ST_PROFILING_RENDERDATA_ADDINDICES(filter.mesh->getIndicesCount());
						ST_PROFILING_RENDERDATA_ADDVERTICES(filter.mesh->getVerticesCount());
					}
				}
			}
			{
				auto view = m_registry.view<TransformComponent, SpriteRendererComponent>();
				for (auto gameObject : view)
				{
					ST_PROFILING_SCOPE("Rendering Sprite");
					auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(gameObject);
					if(sprite.texture)
						Renderer2D::drawTexturedQuad(transform,sprite.texture, sprite.tiling, sprite.color);
					else
						Renderer2D::drawQuad(transform, sprite.color);

					ST_PROFILING_RENDERDATA_ADDINDICES(6);
					ST_PROFILING_RENDERDATA_ADDVERTICES(4);
				}
			}
			Renderer2D::endScene();
		}
		//demo render
		{
			auto view = m_registry.view<CameraComponent>();
			for (auto gameObject : view) {
				auto& cam = m_registry.get<CameraComponent>(gameObject);

				switch (cam.settings.clearType)
				{
				case CameraComponent::Color:
					RenderCommand::setClearColor(cam.settings.clearColor);
					break;
				case CameraComponent::Skybox:
					RenderCommand::setClearColor(glm::vec4(.0f));
					break;
				}

				renderScene(gameObject);
			}
		}
	}
	void Scene::onUpdateRuntime(Timestep ts) {
		ST_PROFILING_FUNCTION();
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, auto& behaviour) {
			if (!behaviour.instance) {
				behaviour.instance = behaviour.InstantiateBehaviour();
				behaviour.instance->gameObject = GameObject{ gameObject, this };
				behaviour.instance->start();
			}
			behaviour.instance->update(ts);
		});

		//calculations
		calculateLights();
		//rendering
		auto view = m_registry.view<CameraComponent>();
		for (auto gameObject : view) {
			auto& cam = m_registry.get<CameraComponent>(gameObject);

			switch (cam.settings.clearType)
			{
			case CameraComponent::Color:
				RenderCommand::setClearColor(cam.settings.clearColor);
				break;
			case CameraComponent::Skybox:
				RenderCommand::setClearColor(glm::vec4(.0f));
				break;
			}

			renderScene(gameObject);
		}
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
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
		ST_PROFILING_FUNCTION();
		auto view = m_registry.view<GameObjectBaseComponent, CameraComponent>();
		for (auto gameObject : view) {
			const auto& base = view.get<GameObjectBaseComponent>(gameObject);
			if (base.tag == "MainCam") {
				return GameObject{ gameObject,this };
			}
		}
		return GameObject::null;
	}
	
	void Scene::calculateLights() {
		ST_PROFILING_FUNCTION();
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
	void Scene::renderScene(entt::entity cam) {
		ST_PROFILING_FUNCTION();
		Renderer2D::beginScene(m_registry.get<CameraComponent>(cam).cam, m_registry.get<TransformComponent>(cam));
		RenderCommand::clear();
		{
			auto group = m_registry.view<MeshFilterComponent>();
			for (auto gameObject : group) {
				ST_PROFILING_SCOPE("Rendering Mesh");
				auto& filter = group.get<MeshFilterComponent>(gameObject);
				auto& ren = m_registry.get<MeshRendererComponent>(gameObject);
				glm::mat4 trans = m_registry.get<TransformComponent>(gameObject);

				for (size_t i = 0; i < filter.mesh->getSubMeshCount(); i++) {
					ST_PROFILING_SCOPE("Rendering SubMesh");
					Renderer::submit(filter.mesh->getSubMesh(i).getVertexArray(), ren.shader, trans);
				}
				if (filter.mesh->getVertexArray() != nullptr) {
					Renderer::submit(filter.mesh->getVertexArray(), ren.shader, trans);
				}
			}
		}
		{
			auto view = m_registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto gameObject : view)
			{
				ST_PROFILING_SCOPE("Rendering Sprite");
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(gameObject);
				if (sprite.texture)
					Renderer2D::drawTexturedQuad(transform, sprite.texture, sprite.tiling, sprite.color);
				else
					Renderer2D::drawQuad(transform, sprite.color);
			}
		}
		Renderer2D::endScene();
	}

	template<typename T>
	void Scene::onComponentAdded(GameObject gameObject, T& component) {  }
	template<>
	void Scene::onComponentAdded<GameObjectBaseComponent>(GameObject gameObject, GameObjectBaseComponent& component) { }
	template<>
	void Scene::onComponentAdded<TransformComponent>(GameObject gameObject, TransformComponent& component) { component.gameObject = gameObject; }
	template<>
	void Scene::onComponentAdded<CameraComponent>(GameObject gameObject, CameraComponent& component) { if (m_viewportWidth > 0 && m_viewportHeight > 0)component.onResize(m_viewportWidth, m_viewportHeight); }
	template<>
	void Scene::onComponentAdded<LightComponent>(GameObject gameObject, LightComponent& component) { }
	template<>
	void Scene::onComponentAdded<MeshFilterComponent>(GameObject gameObject, MeshFilterComponent& component) { gameObject.addComponent<MeshRendererComponent>().shader = m_shaderLib.get("pbr"); }
	template<>
	void Scene::onComponentAdded<MeshRendererComponent>(GameObject gameObject, MeshRendererComponent& component) { }
	template<>
	void Scene::onComponentAdded<SpriteRendererComponent>(GameObject gameObject, SpriteRendererComponent& component) { }
	template<>
	void Scene::onComponentAdded<NativeBehaviourComponent>(GameObject gameObject, NativeBehaviourComponent& component) { }
}
