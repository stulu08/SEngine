#include "st_pch.h"
#include "Scene.h"

#include "Stulu/Scene/SceneRenderer.h"
#include "Stulu/Scene/physx/PhysX.h"
#include "Stulu/Scene/SceneCamera.h"
#include "Stulu/Scene/Components.h"
#include "Stulu/Scene/Behavior.h"
#include "Stulu/Math/Math.h"
#include "Stulu/Core/Time.h"
#include "GameObject.h"

#define PX_PHYSX_STATIC_LIB
#include "PxConfig.h"
#include "physx/PxPhysicsAPI.h"

namespace Stulu {
	Scene::Scene() {
		ST_PROFILING_FUNCTION();
		SceneRenderer::init(this);
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
		calculateLights();
		if (camera.getCamera()) {
			ST_PROFILING_SCOPE("Scene Camera Rendering");
			SceneRenderer::beginScene(camera);
			{
				auto group = m_registry.view<CameraComponent, SkyBoxComponent>();
				for (auto gameObject : group) {
					if (group.get<CameraComponent>(gameObject).settings.clearType == CameraComponent::Skybox) {
						SceneRenderer::drawSkyBox(group.get<SkyBoxComponent>(gameObject));
					}
				}
			}
			{
				auto group = m_registry.view<MeshFilterComponent, TransformComponent, MeshRendererComponent>();
				for (auto gameObject : group) {
					ST_PROFILING_SCOPE("Rendering Mesh Editor");
					SceneRenderer::submit(group.get<MeshRendererComponent>(gameObject), group.get<MeshFilterComponent>(gameObject), group.get<TransformComponent>(gameObject));
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

				}
			}
			SceneRenderer::endScene();
		}
	}

	void Scene::onRuntimeStart() {
		ST_PROFILING_FUNCTION();
		Time::time = .0f;
		//create rigidbodys3d
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
	}
	void Scene::onUpdateRuntime(Timestep ts) {
		ST_PROFILING_FUNCTION();
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, NativeBehaviourComponent& behaviour) {
			if (!behaviour.instance) {
				behaviour.instance = behaviour.InstantiateBehaviour();
				behaviour.instance->gameObject = GameObject{ gameObject, this };
				behaviour.instance->onAwake();
			}
			behaviour.instance->onUpdate();
		});
		Time::time += ts;
		updatedLightsThisFrame = false;
		if(m_data.enablePhsyics3D)
			updatePhysics();
		//calculations
		if (!updatedLightsThisFrame) {
			calculateLights();
		}
		//rendering
		auto view = m_registry.view<CameraComponent>();
		ST_PROFILING_RENDERDATA_RESET();
		for (auto gameObject : view) {
			renderScene(gameObject,ts);
		}
	}
	void Scene::renderScene(entt::entity cam, Timestep ts) {
		ST_PROFILING_FUNCTION();
		ST_PROFILING_RENDERDATA_BEGIN();
		ST_PROFILING_RENDERDATA_ADDCAMERAS(1);

		SceneRenderer::beginScene({ cam,this });
		{
			ST_PROFILING_SCOPE("Rendering Mesh");
			auto group = m_registry.view<MeshFilterComponent, TransformComponent, MeshRendererComponent>();
			for (auto gameObject : group) {
				auto& filter = group.get<MeshFilterComponent>(gameObject);
				SceneRenderer::submit(group.get<MeshRendererComponent>(gameObject), filter, group.get<TransformComponent>(gameObject));
				ST_PROFILING_RENDERDATA_ADDINDICES(filter.mesh.mesh->getIndicesCount());
				ST_PROFILING_RENDERDATA_ADDVERTICES(filter.mesh.mesh->getVerticesCount());
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

				ST_PROFILING_RENDERDATA_ADDINDICES(6);
				ST_PROFILING_RENDERDATA_ADDVERTICES(4);
			}
		}
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, NativeBehaviourComponent& behaviour) {
			if (behaviour.instance)
				behaviour.instance->onRender(ts);
			});
		SceneRenderer::endScene();
		ST_PROFILING_RENDERDATA_END();
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
			m_physics.shutDown();
		}
	}

	void Scene::calculateLights() {
		ST_PROFILING_FUNCTION();
		SceneRenderer::calculateLights();
		SceneRenderer::uploadBuffers(m_data);
		updatedLightsThisFrame = true;
	}
	void Scene::setupPhysics() {
		m_physics.startUp(m_data.physicsData);
		for (auto id : m_registry.view<BoxColliderComponent>()) {
			GameObject object = { id, this };
			glm::mat4 trans = object.getComponent<TransformComponent>().getWorldSpaceTransform();
			glm::vec3 scale, pos, rot;
			Math::decomposeTransform(trans, pos, rot, scale);

			auto& bCollide = object.getComponent<BoxColliderComponent>();

			physx::PxRigidActor* actor;
			if (object.hasComponent<RigidbodyComponent>()) {
				actor = m_physics.createActor(object.getComponent<RigidbodyComponent>(), pos, rot);
			}
			else {
				actor = m_physics.getPhysics()->createRigidStatic(physx::PxTransform(PhysicsVec3fromglmVec3(pos), PhysicsQuatfromglmQuat(rot)));
				actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, true);
			}
			physx::PxTransform relativePose(PhysicsVec3fromglmVec3(bCollide.offset));
			physx::PxMaterial* aMaterial = m_physics.getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);
			physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxBoxGeometry(PhysicsVec3fromglmVec3(scale * bCollide.size)), *aMaterial);
			shape->setLocalPose(relativePose);
			bCollide.shape = (void*)shape;
			m_physics.getScene()->addActor(*actor);
		}
		for (auto id : m_registry.view<SphereColliderComponent>()) {
			GameObject object = { id, this };
			glm::mat4 trans = object.getComponent<TransformComponent>().getWorldSpaceTransform();
			glm::vec3 scale, pos, rot;
			Math::decomposeTransform(trans, pos, rot, scale);

			auto& bCollide = object.getComponent<SphereColliderComponent>();

			physx::PxRigidActor* actor;
			if (object.hasComponent<RigidbodyComponent>()) {
				actor = m_physics.createActor(object.getComponent<RigidbodyComponent>(), pos, rot);
			}
			else {
				actor = m_physics.getPhysics()->createRigidStatic(physx::PxTransform(PhysicsVec3fromglmVec3(pos), PhysicsQuatfromglmQuat(rot)));
				actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, true);
			}
			physx::PxTransform relativePose(PhysicsVec3fromglmVec3(bCollide.offset));
			physx::PxMaterial* aMaterial = m_physics.getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);
			physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxSphereGeometry(scale.x * bCollide.radius), *aMaterial);
			shape->setLocalPose(relativePose);
			bCollide.shape = (void*)shape;
			m_physics.getScene()->addActor(*actor);
		}
		for (auto id : m_registry.view<MeshColliderComponent>()) {
			GameObject object = { id, this };
			glm::mat4 trans = object.getComponent<TransformComponent>().getWorldSpaceTransform();
			glm::vec3 scale, pos, rot;
			Math::decomposeTransform(trans, pos, rot, scale);

			auto& bCollide = object.getComponent<MeshColliderComponent>();

			if (!object.hasComponent<MeshFilterComponent>())
				continue;
			Ref<Mesh>& mesh = object.getComponent<MeshFilterComponent>().mesh.mesh;
			if (mesh == nullptr)
				continue;

			physx::PxRigidActor* actor = m_physics.getPhysics()->createRigidStatic(physx::PxTransform(PhysicsVec3fromglmVec3(pos), PhysicsQuatfromglmQuat(rot)));

			physx::PxMaterial* aMaterial = m_physics.getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);
			physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxTriangleMeshGeometry(m_physics.createTriangleMesh(mesh),
				physx::PxMeshScale(PhysicsVec3fromglmVec3(scale))), *aMaterial);
			m_physics.getScene()->addActor(*actor);
		}
	}
	void Scene::updatePhysics() {
		ST_PROFILING_FUNCTION();
		m_physics.getScene()->simulate(Time::deltaTime);
		//we do as much ass possible here
		while (!m_physics.getScene()->fetchResults()) {
			if (!updatedLightsThisFrame) {
				calculateLights();
			}
		}
		auto view = m_registry.view<RigidbodyComponent>();
		for (auto id : view) {
			GameObject object = { id, this };
			auto& rb = object.getComponent<RigidbodyComponent>();
			physx::PxRigidActor* actor = (physx::PxRigidActor*)rb.body;

			if (rb.rbType == RigidbodyComponent::Type::Dynamic) {
				auto& tc = object.getComponent<TransformComponent>();
				physx::PxTransform tr = actor->getGlobalPose();

				if (rb.rotationX && rb.rotationY && rb.rotationZ) {
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), PhysicsVec3toglmVec3(tr.p)) * glm::toMat4(glm::quat(tr.q.w, tr.q.x, tr.q.y, tr.q.z));
					if (tc.parent) {
						glm::vec3 worldPos, worldRotation, worldScale;
						Math::decomposeTransform(transform, worldPos, worldRotation, worldScale);
						glm::mat4 parent = tc.parent.getComponent<TransformComponent>().getWorldSpaceTransform();
						const glm::vec3 parentPos = glm::vec3(parent[3]);
						parent[3] = glm::vec4(0, 0, 0, parent[3].w);
						transform = glm::translate(glm::mat4(1.0f), worldPos - parentPos) * glm::toMat4(glm::quat(worldRotation)) * glm::scale(glm::mat4(1.0f), worldScale) / parent;
					}
					glm::vec3 pos, rotation, scale;
					Math::decomposeTransform(transform, pos, rotation, scale);

					tc.position = pos;
					tc.rotation = rotation;
				}
				else {
					tc.position = PhysicsVec3toglmVec3(tr.p);
				}
			}
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
