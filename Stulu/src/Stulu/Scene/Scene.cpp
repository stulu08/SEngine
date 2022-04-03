#include "st_pch.h"
#include "Scene.h"

#include "Stulu/Scene/SceneRenderer.h"
#include "Stulu/Scene/physx/PhysX.h"
#include "Stulu/Scene/SceneCamera.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Scene/Behavior.h"
#include "Stulu/ScriptCore/ScriptCore.h"
#include "Stulu/Math/Math.h"
#include "Stulu/Core/Time.h"
#include "GameObject.h"

#define PX_PHYSX_STATIC_LIB
#include "PxConfig.h"
#include "physx/PxPhysicsAPI.h"

namespace Stulu {
	static bool startedPhysics = false;
	Scene::Scene() {
		ST_PROFILING_FUNCTION();
		SceneRenderer::init(this);
		if(!s_physics.started())
			s_physics.startUp();
	}
	Scene::~Scene() {
		m_registry.clear();
	}
	GameObject Scene::createGameObject(const std::string& name, UUID uuid) {
		ST_PROFILING_FUNCTION();
		GameObject go = { m_registry.create(), this };
		auto& base = go.addComponent<GameObjectBaseComponent>(!name.empty() ? name : "GameObject");
		base.uuid = uuid;

		go.addComponent<TransformComponent>();
		return go;
	}
	void Scene::destroyGameObject(GameObject gameObject) {
		ST_PROFILING_FUNCTION();
		if (gameObject.hasComponent<NativeBehaviourComponent>()) {
			auto& b = gameObject.getComponent<NativeBehaviourComponent>();
			if (b.instance) {
				b.instance->onDestroy();
				gameObject.getComponent<NativeBehaviourComponent>().DestroyBehaviour(&b);
			}
		}
		//destroy all childs
		m_registry.view<TransformComponent>().each([=](entt::entity go, TransformComponent& transform) {
			if (transform.parent == gameObject)
				destroyGameObject({ go, this });
			});
		m_registry.destroy(gameObject);
		gameObject = GameObject::null;
	}

	void Scene::onUpdateEditor(Timestep ts, const SceneCamera& camera) {
		ST_PROFILING_FUNCTION();
		ST_PROFILING_RENDERDATA_RESET();
		ST_PROFILING_RENDERDATA_BEGIN();
		s_activeScene = this;
		updateAllTransforms();
		SceneRenderer::calculateLights();

		if (camera.getCamera()) {
			ST_PROFILING_SCOPE("Scene Camera Rendering");
			SceneRenderer::beginScene(camera);
			SceneRenderer::uploadBuffers(m_data);
			{
				auto group = m_registry.view<MeshFilterComponent, TransformComponent, MeshRendererComponent>();
				for (auto gameObject : group) {
					ST_PROFILING_SCOPE("Rendering Mesh Editor");
					SceneRenderer::submit(group.get<MeshRendererComponent>(gameObject), group.get<MeshFilterComponent>(gameObject), group.get<TransformComponent>(gameObject));
					group.get<MeshRendererComponent>(gameObject).m_enabledStencilBufferNextFrame = false;
					ST_PROFILING_RENDERDATA_ADDINDICES(group.get<MeshFilterComponent>(gameObject).mesh.mesh->getIndicesCount());
					ST_PROFILING_RENDERDATA_ADDVERTICES(group.get<MeshFilterComponent>(gameObject).mesh.mesh->getVerticesCount());
				}
			}
			{
				auto view = m_registry.view<TransformComponent, SpriteRendererComponent>();
				for (auto gameObject : view)
				{
					ST_PROFILING_SCOPE("Rendering Sprite");
					auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(gameObject);
					if(sprite.texture)
						Renderer2D::drawTexturedQuad(transform, sprite.texture, sprite.texture->getSettings().tiling * sprite.tiling, sprite.color);
					else
						Renderer2D::drawQuad(transform, sprite.color);

					ST_PROFILING_RENDERDATA_ADDINDICES(6);
					ST_PROFILING_RENDERDATA_ADDVERTICES(4);
				}
			}
			SceneRenderer::endScene();
		}
		ST_PROFILING_RENDERDATA_END();
	}

	void Scene::onRuntimeStart() {
		ST_PROFILING_FUNCTION();
		Time::time = .0f;
		//create rigidbodys3d
		updateAllTransforms();
		if (m_data.enablePhsyics3D) {
			setupPhysics();
		}
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, NativeBehaviourComponent& behaviour) {
			behaviour.instance = behaviour.InstantiateBehaviour();
			behaviour.instance->gameObject = GameObject{ gameObject, this };
			behaviour.instance->onAwake();
		});
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, NativeBehaviourComponent& behaviour) {
			if (!behaviour.instance) {
				behaviour.instance = behaviour.InstantiateBehaviour();
				behaviour.instance->gameObject = GameObject{ gameObject, this };
			}
			behaviour.instance->onStart();
		});
		m_registry.view<CameraComponent>().each([=](auto gameObject, CameraComponent& cam) {
			cam.getFrameBuffer()->getSpecs().textureFormat = (m_data.framebuffer16bit ? TextureSettings::Format::RGBA16F : TextureSettings::Format::RGBA);
			cam.getFrameBuffer()->resize(cam.settings.textureWidth, cam.settings.textureHeight);
			});
	}
	void Scene::onUpdateRuntime(Timestep ts) {
		ST_PROFILING_FUNCTION();
		s_activeScene = this;
		Time::time += ts;
		updateAllTransforms();
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, NativeBehaviourComponent& behaviour) {
			if (!behaviour.instance) {
				behaviour.instance = behaviour.InstantiateBehaviour();
				behaviour.instance->gameObject = GameObject{ gameObject, this };
				behaviour.instance->onAwake();
			}
			behaviour.instance->onUpdate();
		});

		//calculations
		if(m_data.enablePhsyics3D)
			updatePhysics();
		else
			SceneRenderer::calculateLights();//otherwise we do it in m_physics.getScene()->fetchResults()
		//rendering
		auto view = m_registry.view<CameraComponent>();
		for (auto gameObject : view) {
			renderScene(gameObject,ts);
		}
	}
	void Scene::renderScene(entt::entity cam, Timestep ts) {
		ST_PROFILING_FUNCTION();

		SceneRenderer::beginScene({ cam,this });
		SceneRenderer::uploadBuffers(m_data);
		{
			ST_PROFILING_SCOPE("Rendering Mesh");
			auto group = m_registry.view<MeshFilterComponent, TransformComponent, MeshRendererComponent>();
			for (auto gameObject : group) {
				auto& filter = group.get<MeshFilterComponent>(gameObject);
				SceneRenderer::submit(group.get<MeshRendererComponent>(gameObject), filter, group.get<TransformComponent>(gameObject));
			}
		}
		{
			auto view = m_registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto gameObject : view)
			{
				ST_PROFILING_SCOPE("Rendering Sprite");
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(gameObject);
				if (sprite.texture)
					Renderer2D::drawTexturedQuad(transform, sprite.texture, sprite.tiling * sprite.texture->getSettings().tiling, sprite.color);
				else
					Renderer2D::drawQuad(transform, sprite.color);
			}
		}
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, NativeBehaviourComponent& behaviour) {
			if (behaviour.instance)
				behaviour.instance->onRender(ts);
			});
		SceneRenderer::endScene();
	}
	void Scene::onRuntimeStop() {
		ST_PROFILING_FUNCTION();
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, NativeBehaviourComponent& behaviour) {
			if (behaviour.instance) {
				behaviour.instance->onSceneExit();
				behaviour.instance->onDestroy();
				behaviour.DestroyBehaviour(&behaviour);
			}
			});

		if (m_data.enablePhsyics3D) {
			s_physics.releasePhysics();
			for (auto id : m_registry.view<RigidbodyComponent>()) {
				GameObject object = { id, this };
				object.getComponent<RigidbodyComponent>().destroy(object, s_physics);
			}
			for (auto id : m_registry.view<BoxColliderComponent>()) {
				GameObject object = { id, this };
				object.getComponent<BoxColliderComponent>().destroy(object, s_physics);
			}
			for (auto id : m_registry.view<SphereColliderComponent>()) {
				GameObject object = { id, this };
				object.getComponent<SphereColliderComponent>().destroy(object, s_physics);
			}
			for (auto id : m_registry.view<MeshColliderComponent>()) {
				GameObject object = { id, this };
				object.getComponent<MeshColliderComponent>().destroy(object, s_physics);
			}
		}
	}
	void Scene::setupPhysics() {
		s_physics.createPhysics(m_data.physicsData);
		for (auto id : m_registry.view<BoxColliderComponent>()) {
			GameObject object = { id, this };
			object.getComponent<BoxColliderComponent>().create(object, s_physics);
		}
		for (auto id : m_registry.view<SphereColliderComponent>()) {
			GameObject object = { id, this };
			object.getComponent<SphereColliderComponent>().create(object, s_physics);
		}
		for (auto id : m_registry.view<MeshColliderComponent>()) {
			GameObject object = { id, this };
			object.getComponent<MeshColliderComponent>().create(object, s_physics);
		}
	}
	void Scene::updatePhysics() {
		ST_PROFILING_FUNCTION();
		if (Time::deltaTime == 0.0f)
			return;
		s_physics.getScene()->simulate(Time::deltaTime);
		//we do as much ass possible here
		while (!s_physics.getScene()->fetchResults()) {
			SceneRenderer::calculateLights();
		}
		auto view = m_registry.view<RigidbodyComponent>();
		for (auto id : view) {
			GameObject object = { id, this };
			auto& rb = object.getComponent<RigidbodyComponent>();
			auto& tc = object.getComponent<TransformComponent>();
			if (rb.body == nullptr)
				continue;
			physx::PxRigidActor* actor = (physx::PxRigidActor*)rb.body;
			if (rb.rbType == RigidbodyComponent::Type::Dynamic) {
				physx::PxTransform tr = actor->getGlobalPose();

				glm::vec3 pos = PhysicsVec3toglmVec3(tr.p);
				glm::quat rot = glm::quat(tr.q.w, tr.q.x, tr.q.y, tr.q.z);
				if (pos != tc.worldPosition || rot != tc.worldRotation) {
					glm::mat4 transform;//new local

					if (tc.parent) {
						auto& parent = tc.parent.getComponent<TransformComponent>();

						transform =
							glm::translate(glm::mat4(1.0f), pos - parent.worldPosition) *
							(glm::toMat4(rot) / glm::toMat4(parent.worldRotation)) *
							glm::scale(glm::mat4(1.0f), tc.scale / parent.worldScale);
						glm::vec3 s;
						Math::decomposeTransform(transform, tc.position, tc.rotation, s);
					}
					else {
						tc.position = pos;
						tc.rotation = rot;
					}
				}
				else {
					actor->setGlobalPose(physx::PxTransform(PhysicsVec3fromglmVec3(tc.worldPosition),PhysicsQuatfromglmQuat(tc.worldRotation)));
				}
			}
			else {
				actor->setGlobalPose(physx::PxTransform(PhysicsVec3fromglmVec3(tc.worldPosition), PhysicsQuatfromglmQuat(tc.worldRotation)));
			}
		}

	}
	std::unordered_map<entt::entity, bool> st_transform_updated;
	void Scene::updateAllTransforms() {
		ST_PROFILING_FUNCTION();
		auto view = m_registry.view<TransformComponent>();
		for (auto id : view) {
			GameObject object = { id, this };
			auto& tc = object.getComponent<TransformComponent>();
			tc.gameObject = object;
			if (st_transform_updated.find(id) == st_transform_updated.end())
				updateTransform(tc);
		}
		st_transform_updated.clear();
	}
	void Scene::updateTransform(TransformComponent& tc) {
		ST_PROFILING_FUNCTION();
		if (tc.parent) {
			TransformComponent& ptc = tc.parent.getComponent<TransformComponent>();
			if (st_transform_updated.find(tc.parent.m_entity) == st_transform_updated.end())
				updateTransform(tc.parent.getComponent<TransformComponent>());

			tc.transform = 
				glm::translate(glm::mat4(1.0f), tc.position + ptc.worldPosition) * 
				(glm::toMat4(tc.rotation) * glm::toMat4(ptc.worldRotation)) * 
				glm::scale(glm::mat4(1.0f), tc.scale * ptc.worldScale);

			Math::decomposeTransform(tc.transform, tc.worldPosition, tc.worldRotation, tc.worldScale);
		}
		else {
			tc.worldPosition = tc.position;
			tc.worldRotation = tc.rotation;
			tc.worldScale = tc.scale;
			tc.transform = Math::createMat4(tc.worldPosition, tc.worldRotation, tc.worldScale);
		}

		tc.forward = glm::rotate(tc.worldRotation, TRANSFORM_FOREWARD_DIRECTION);
		tc.up = glm::rotate(tc.worldRotation, TRANSFORM_UP_DIRECTION);
		tc.right = glm::rotate(tc.worldRotation, TRANSFORM_RIGHT_DIRECTION);

		if(tc.gameObject)
			st_transform_updated[tc.gameObject.m_entity] = true;
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

	void Scene::onApplicationQuit() {
		if (s_physics.started())
			s_physics.shutDown();
	}

	GameObject Scene::addModel(Model& model) {
		ST_PROFILING_FUNCTION();
		std::vector<MeshAsset>& vec = model.getMeshes();
		GameObject root = GameObject::null;
		for (MeshAsset& mesh : vec) {
			if (mesh.parentMeshAsset != UUID::null)
				continue;
			//root node
			root = addMeshAssetsToScene(mesh, model);
			root.getComponent<TransformComponent>().parent = GameObject::null;
		}
		return root;
	}
	GameObject Scene::addMeshAssetsToScene(MeshAsset& mesh, Model& model) {
		ST_PROFILING_FUNCTION();
		GameObject go = createGameObject(mesh.name.c_str() ? mesh.name : "unnamed Mesh");
		auto& tc = go.getComponent<TransformComponent>();
		Math::decomposeTransform(mesh.transform, tc.position, tc.rotation, tc.scale);
		if (mesh.hasMesh) {
			go.addComponent<MeshFilterComponent>().mesh = mesh;
			go.saveAddComponent<MeshRendererComponent>();
		}
		for (MeshAsset& child : model.getMeshes()) {
			if (child.parentMeshAsset == mesh.uuid) {
				GameObject c = addMeshAssetsToScene(child, model);
					go.getComponent<TransformComponent>().addChild(c);
			}
				
		}
		if (mesh.parentMeshAsset == UUID::null)
			go.getComponent<TransformComponent>().parent = GameObject::null;
		return go;
	}
	GameObject Scene::addMeshAssetToScene(MeshAsset& mesh) {
		ST_PROFILING_FUNCTION();
		GameObject go = createGameObject(mesh.name.c_str() ? mesh.name : "unnamed Mesh");
		if (mesh.hasMesh) {
			go.addComponent<MeshFilterComponent>().mesh = mesh;
			go.saveAddComponent<MeshRendererComponent>();
		}
		auto& tc = go.getComponent<TransformComponent>();
		Math::decomposeTransform(mesh.transform, tc.position, tc.rotation, tc.scale);
		return go;
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

	template<typename T>
	void Scene::onComponentAdded(GameObject gameObject, T& component) {  }
	template<>
	void STULU_API Scene::onComponentAdded<GameObjectBaseComponent>(GameObject gameObject, GameObjectBaseComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<TransformComponent>(GameObject gameObject, TransformComponent& component) { component.gameObject = gameObject; }
	template<>
	void STULU_API Scene::onComponentAdded<CameraComponent>(GameObject gameObject, CameraComponent& component) { if (m_viewportWidth > 0 && m_viewportHeight > 0)component.onResize(m_viewportWidth, m_viewportHeight); }
	template<>
	void STULU_API Scene::onComponentAdded<LightComponent>(GameObject gameObject, LightComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<SkyBoxComponent>(GameObject gameObject, SkyBoxComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<MeshFilterComponent>(GameObject gameObject, MeshFilterComponent& component) { if (!gameObject.hasComponent<MeshRendererComponent>()) gameObject.addComponent<MeshRendererComponent>(); }
	template<>
	void STULU_API Scene::onComponentAdded<MeshRendererComponent>(GameObject gameObject, MeshRendererComponent& component) { if (!gameObject.hasComponent<MeshFilterComponent>()) gameObject.addComponent<MeshFilterComponent>(); }
	template<>
	void STULU_API Scene::onComponentAdded<SpriteRendererComponent>(GameObject gameObject, SpriteRendererComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<RigidbodyComponent>(GameObject gameObject, RigidbodyComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<BoxColliderComponent>(GameObject gameObject, BoxColliderComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<SphereColliderComponent>(GameObject gameObject, SphereColliderComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<MeshColliderComponent>(GameObject gameObject, MeshColliderComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<NativeBehaviourComponent>(GameObject gameObject, NativeBehaviourComponent& component) { }

	template<typename T>
	void Scene::onComponentRemove(GameObject gameObject, T& component) {  }
	template<>
	void STULU_API Scene::onComponentRemove<GameObjectBaseComponent>(GameObject gameObject, GameObjectBaseComponent& component) { }
	template<>
	void STULU_API Scene::onComponentRemove<TransformComponent>(GameObject gameObject, TransformComponent& component) {  }
	template<>
	void STULU_API Scene::onComponentRemove<CameraComponent>(GameObject gameObject, CameraComponent& component) { component.cam.reset(); }
	template<>
	void STULU_API Scene::onComponentRemove<LightComponent>(GameObject gameObject, LightComponent& component) { }
	template<>
	void STULU_API Scene::onComponentRemove<SkyBoxComponent>(GameObject gameObject, SkyBoxComponent& component) { }
	template<>
	void STULU_API Scene::onComponentRemove<MeshFilterComponent>(GameObject gameObject, MeshFilterComponent& component) {  }
	template<>
	void STULU_API Scene::onComponentRemove<MeshRendererComponent>(GameObject gameObject, MeshRendererComponent& component) { }
	template<>
	void STULU_API Scene::onComponentRemove<SpriteRendererComponent>(GameObject gameObject, SpriteRendererComponent& component) { }
	template<>
	void STULU_API Scene::onComponentRemove<RigidbodyComponent>(GameObject gameObject, RigidbodyComponent& component) { }
	template<>
	void STULU_API Scene::onComponentRemove<BoxColliderComponent>(GameObject gameObject, BoxColliderComponent& component) { }
	template<>
	void STULU_API Scene::onComponentRemove<SphereColliderComponent>(GameObject gameObject, SphereColliderComponent& component) { }
	template<>
	void STULU_API Scene::onComponentRemove<MeshColliderComponent>(GameObject gameObject, MeshColliderComponent& component) { }
	template<>
	void STULU_API Scene::onComponentRemove<NativeBehaviourComponent>(GameObject gameObject, NativeBehaviourComponent& component) { }
}
