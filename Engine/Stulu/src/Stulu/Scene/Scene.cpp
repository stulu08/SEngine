#include "st_pch.h"
#include "Scene.h"

#include "Stulu/Events/Event.h"
#include "Stulu/Scene/SceneRenderer.h"
#include "Stulu/Scene/physx/PhysX.h"
#include "Stulu/Scene/SceneCamera.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Math/Math.h"
#include "Stulu/Core/Time.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Core/Input.h"
#include "Stulu/Scripting/EventCaller.h"
#include "GameObject.h"

#include "PxConfig.h"
#include "PxPhysicsAPI.h"

namespace Stulu {
	Scene::Scene() {
		m_renderer = createScope<SceneRenderer>(this);
		if(Application::get().getAssemblyManager())
			m_caller = createRef<EventCaller>(this);
	}
	Scene::Scene(const SceneData data) 
		: m_data(data) {
		m_renderer = createScope<SceneRenderer>(this);
		if (Application::get().getAssemblyManager())
			m_caller = createRef<EventCaller>(this);
	}
	Scene::~Scene() {
		m_uuidGameObjectMap.clear();
		m_registry.clear();
	}

	GameObject Scene::createGameObject(UUID uuid) {
		return createGameObject("GameObject", uuid);
	}

	GameObject Scene::createGameObject(const std::string& name, UUID uuid, uint32_t id) {
		GameObject go;
		if(id != UINT32_MAX)
			go = { m_registry.create((entt::entity)id), this };
		else
			go = { m_registry.create(), this };

		auto& base = go.addComponent<GameObjectBaseComponent>(!name.empty() ? name : "GameObject", uuid);
		base.updateUUID(uuid);
		go.addComponent<TransformComponent>();
		m_caller->GameObjectCreate(go);
		return go;
	}
	void Scene::destroyGameObject(GameObject gameObject) {
		if (!gameObject) {
			CORE_ERROR("Cant destroy GameObject null");
			return;
		}
		m_caller->GameObjectDestory(gameObject);
		m_caller->onDestroy(gameObject);

		if (m_data.enablePhsyics3D && gameObject.hasComponent<RigidbodyComponent>()) {
			gameObject.removeComponent<RigidbodyComponent>();
		}

		//destroy all childs
		m_registry.view<TransformComponent>().each([=](entt::entity go, TransformComponent& transform) {
			if (transform.parent == gameObject)
				destroyGameObject({ go, this });
			});

		m_uuidGameObjectMap.erase(gameObject.getId());
		m_registry.destroy(gameObject);

		gameObject.m_entity = entt::null;
		gameObject.m_scene = nullptr;
		gameObject = GameObject::null;
	}

	void Scene::onUpdateEditor(Timestep ts, SceneCamera& camera, bool render) {
		ST_PROFILING_SCOPE("Scene - Editor Update");
		s_activeScene = this;
		updateAllTransforms();
		m_renderer->LoadLights();
		if (camera.getCamera() && render) {
			renderSceneEditor(camera);
		}
	}
	void Scene::onUpdateRuntime(Timestep ts, bool render) {
		ST_PROFILING_SCOPE("Scene - Runtime Update");
		s_activeScene = this;
		Time::time += ts;

		if (m_firstRuntimeUpdate)
			m_caller->onStart();

		//calculations
		if (m_data.enablePhsyics3D)
			updatePhysics();
		else
			runtime_updatesetups();
		//rendering
		if(render) {
			renderScene();
		}
		m_firstRuntimeUpdate = false;
	}
	void Scene::runtime_updatesetups() {
		updateAllTransforms();
		m_caller->onUpdate();
		
		m_renderer->LoadLights();
		setupSceneForRendering(true);
	}
	void Scene::renderScene(bool callEvents) {
		s_activeScene = this;
		ST_PROFILING_SCOPE("Scene - Runtime Render");
		//setupSceneForRendering(callEvents); //we do it while waiting for physx
		auto& view = m_registry.view<CameraComponent>();
		for (auto goID : view) {
			renderSceneForCamera(GameObject(goID, this), callEvents);
		}
		closeSceneForRendering();
	}

	void Scene::setupSceneForRendering(bool callEvents) {
		s_activeScene = this;
		m_renderer->Begin();
		//register all objects
		{
			auto group = m_registry.view<MeshFilterComponent, TransformComponent, MeshRendererComponent>();
			for (auto gameObject : group) {
				if (group.get<MeshFilterComponent>(gameObject).mesh.mesh) {
					m_renderer->RegisterObject(group.get<MeshRendererComponent>(gameObject), group.get<MeshFilterComponent>(gameObject), group.get<TransformComponent>(gameObject));
				}
			}
		}
		if(callEvents) {
			m_caller->onRender();
		}
	}

	void Scene::renderSceneForCamera(GameObject go, bool callEvents) {
		s_activeScene = this;
		CameraComponent& cameraComp = go.getComponent<CameraComponent>();
		TransformComponent& transformComp = go.getComponent<TransformComponent>();
		if (cameraComp.mode == CameraMode::Perspective) {
			cameraComp.frustum = VFC::createFrustum(cameraComp.settings.aspectRatio, cameraComp.settings.zNear,
				cameraComp.settings.zFar, cameraComp.settings.fov, transformComp);
		}
		else if (cameraComp.mode == CameraMode::Orthographic) {
			cameraComp.frustum = VFC::createFrustum_ortho(-cameraComp.settings.aspectRatio * cameraComp.settings.zoom, 
				cameraComp.settings.aspectRatio * cameraComp.settings.zoom, -cameraComp.settings.zoom, cameraComp.settings.zoom, 
				cameraComp.settings.zNear, cameraComp.settings.zFar, transformComp);
		}
		Renderer::uploadCameraBufferData(cameraComp.getNativeCamera()->getProjectionMatrix(), glm::inverse(transformComp.transform),
			transformComp.worldPosition, transformComp.eulerAnglesDegrees, cameraComp.settings.zNear, cameraComp.settings.zFar);

		//clear 
		cameraComp.getNativeCamera()->bindFrameBuffer();
		m_renderer->Clear(cameraComp);
		cameraComp.getNativeCamera()->unbindFrameBuffer();

		//draw 3D
		m_renderer->DrawSceneToCamera(transformComp, cameraComp);
		//draw 2D stuff
		cameraComp.getNativeCamera()->bindFrameBuffer();
		Renderer2D::begin();
		if (callEvents) {
			m_caller->onRender2D();
		}

		m_renderer->drawAll2d(transformComp);
		Renderer2D::flush();
		cameraComp.getNativeCamera()->unbindFrameBuffer();
	}
	void Scene::closeSceneForRendering() {
		s_activeScene = this;
		m_renderer->End();
	}

	void Scene::renderSceneEditor(SceneCamera& camera) {
		ST_PROFILING_SCOPE("Scene - Editor Render");

		s_activeScene = this;
		m_renderer->Begin();
		{
			auto group = m_registry.view<MeshFilterComponent, TransformComponent, MeshRendererComponent>();
			for (auto gameObject : group) {
				if (group.get<MeshFilterComponent>(gameObject).mesh.mesh) {
					m_renderer->RegisterObject(group.get<MeshRendererComponent>(gameObject), group.get<MeshFilterComponent>(gameObject), group.get<TransformComponent>(gameObject));
				}
			}
		}
		GameObject mc = getMainCamera();

		Renderer::uploadCameraBufferData(camera.getCamera()->getProjectionMatrix(), glm::inverse(camera.getTransform().transform),
			camera.getTransform().worldPosition, camera.getTransform().eulerAnglesDegrees, camera.getNear(), camera.getFar());
		//clear 
		camera.getCamera()->bindFrameBuffer();
		if (mc)
			m_renderer->Clear(mc.getComponent<CameraComponent>());
		else
			m_renderer->Clear();

		//only in editor
		if ((m_data.shaderFlags & ST_ShaderViewFlags_DisplayVertices))
			RenderCommand::setWireFrame(true);

		camera.getCamera()->unbindFrameBuffer();

		//draw 3D
		if (mc)
			m_renderer->DrawSceneToCamera(camera, mc.getComponent<CameraComponent>());
		else
			m_renderer->DrawSceneToCamera(camera);

		//draw 2D stuff
		camera.getCamera()->bindFrameBuffer();
		Renderer2D::begin();
		m_renderer->drawAll2d(camera.getTransform());
		Renderer2D::flush();
		camera.getCamera()->unbindFrameBuffer();

		//only in editor
		if ((m_data.shaderFlags & ST_ShaderViewFlags_DisplayVertices))
			RenderCommand::setWireFrame(false);

		m_renderer->End();
		m_renderer->ApplyPostProcessing(camera);
	}

	void Scene::onRuntimeStart() {
		s_activeScene = this;
		Time::time = .0f;
		m_firstRuntimeUpdate = true;
		updateAllTransforms();
		if (m_data.enablePhsyics3D) {
			setupPhysics();
		}
		m_caller->ConstructManaged();

		m_registry.view<CameraComponent>().each([=](auto gameObject, CameraComponent& cam) {
			cam.getFrameBuffer()->resize(cam.settings.textureWidth, cam.settings.textureHeight);
			});

		for (entt::entity goID : getAllGameObjectsWith<MeshRendererComponent>()) {
			if(m_registry.get<MeshRendererComponent>(goID).material)
				m_registry.get<MeshRendererComponent>(goID).material->uploadData();
		}

		Time::deltaTime = .0f;
	}
	void Scene::onRuntimeStop() {
		s_activeScene = this;

		m_caller->onSceneExit();
		m_caller->onDestroy();

		Input::setCursorMode(Input::CursorMode::Normal);
		if (m_data.enablePhsyics3D) {
			m_physics->releasePhysics();
		}
	}

	void Scene::setupPhysics() {
		if (!PhysX::started())
			PhysX::startUp();
		m_physics = createScope<PhysX>();
		
		m_physics->createPhysics(m_data.physicsData);
		for (auto id : m_registry.view<BoxColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<BoxColliderComponent>().rigidbody == nullptr)
				object.getComponent<BoxColliderComponent>().create(object, m_physics);
		}
		for (auto id : m_registry.view<SphereColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<SphereColliderComponent>().rigidbody == nullptr)
				object.getComponent<SphereColliderComponent>().create(object, m_physics);
		}
		for (auto id : m_registry.view<CapsuleColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<CapsuleColliderComponent>().rigidbody == nullptr)
				object.getComponent<CapsuleColliderComponent>().create(object, m_physics);
		}
		for (auto id : m_registry.view<MeshColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<MeshColliderComponent>().rigidbody == nullptr)
				object.getComponent<MeshColliderComponent>().create(object, m_physics);
		}
	}
	void Scene::updatePhysics() {
		if (Time::deltaTime == 0.0f || m_firstRuntimeUpdate) {
			return;
		}
		ST_PROFILING_SCOPE("Scene - PhysX Update");
		{
			ST_PROFILING_SCOPE("Scene - Waiting for PhysX");
			bool alreadyRun = false;
			m_physics->getScene()->simulate(Time::deltaTime);
			//we do as much as possible here
			while (!m_physics->getScene()->fetchResults()) {
				if (!alreadyRun) {
					runtime_updatesetups();
					alreadyRun = true;
				}
			}
		}
		for (auto id : m_registry.view<BoxColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<BoxColliderComponent>().rigidbody == nullptr)
				object.getComponent<BoxColliderComponent>().create(object, m_physics);
		}
		for (auto id : m_registry.view<SphereColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<SphereColliderComponent>().rigidbody == nullptr)
				object.getComponent<SphereColliderComponent>().create(object, m_physics);
		}
		for (auto id : m_registry.view<CapsuleColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<CapsuleColliderComponent>().rigidbody == nullptr)
				object.getComponent<CapsuleColliderComponent>().create(object, m_physics);
		}
		for (auto id : m_registry.view<MeshColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<MeshColliderComponent>().rigidbody == nullptr)
				object.getComponent<MeshColliderComponent>().create(object, m_physics);
		}
		auto view = m_registry.view<RigidbodyComponent>();
		for (auto id : view) {
			GameObject object = { id, this };
			auto& rb = object.getComponent<RigidbodyComponent>();
			auto& tc = object.getComponent<TransformComponent>();
			if (rb.body == nullptr) {
				continue;
			}
			physx::PxRigidActor* actor = (physx::PxRigidActor*)rb.body;
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
		}

	}

	std::unordered_map<entt::entity, bool> st_transform_updated;
	void Scene::updateAllTransforms() {
		ST_PROFILING_SCOPE("Scene - Transform Update");

		st_transform_updated.clear();
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
		tc.eulerAnglesDegrees = glm::degrees(Math::QuaternionToEuler(tc.rotation));
		tc.eulerAnglesWorldDegrees = glm::degrees(Math::QuaternionToEuler(tc.worldRotation));

		if(tc.gameObject)
			st_transform_updated[tc.gameObject.m_entity] = true;
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height) {
		m_viewportWidth = width;
		m_viewportHeight = height;
		auto view = m_registry.view<CameraComponent>();
		for (auto entity : view) {
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.settings.isRenderTarget)
				cameraComponent.onResize(width, height);
			else
				cameraComponent.updateSize();
		}
	}

	void Scene::onEvent(Stulu::Event& e) {
		if (e.getEventType() == EventType::WindowClose) {
			if (!PhysX::started())
				PhysX::shutDown();
		}
		
	}

	GameObject Scene::addModel(Model& model) {
		std::vector<MeshAsset>& vec = model.getMeshes();
		GameObject root = createGameObject(std::filesystem::path(model.getDirectory()).filename().string());
		for (MeshAsset& mesh : vec) {
			if (mesh.parentMeshAsset != UUID::null)
				continue;
			//root node
			destroyGameObject(root);
			root = addMeshAssetsToScene(mesh, model);
			root.getComponent<TransformComponent>().parent = GameObject::null;
		}
		return root;
	}
	GameObject Scene::addMeshAssetsToScene(MeshAsset& mesh, Model& model) {
		GameObject go = createGameObject(mesh.name.c_str() ? mesh.name : "unnamed Mesh");
		auto& tc = go.getComponent<TransformComponent>();
		Math::decomposeTransform(mesh.transform, tc.position, tc.rotation, tc.scale);
		if (mesh.hasMesh) {
			go.addComponent<MeshFilterComponent>().mesh = mesh;
			go.saveAddComponent<MeshRendererComponent>().cullmode = mesh.cullMode;
			if (model.getMaterials().find(mesh.materialIDs[0]) != model.getMaterials().end()) {
				go.getComponent<MeshRendererComponent>().material = std::any_cast<Ref<Material>>(AssetsManager::get(model.getMaterials()[mesh.materialIDs[0]]->getUUID()).data);
			}
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
		GameObject go = createGameObject(mesh.name.c_str() ? mesh.name : "unnamed Mesh");
		if (mesh.hasMesh) {
			go.addComponent<MeshFilterComponent>().mesh = mesh;
			go.saveAddComponent<MeshRendererComponent>().cullmode = mesh.cullMode;
		}
		auto& tc = go.getComponent<TransformComponent>();
		Math::decomposeTransform(mesh.transform, tc.position, tc.rotation, tc.scale);
		return go;
	}
	GameObject Scene::getMainCamera() {
		auto view = m_registry.view<GameObjectBaseComponent, CameraComponent>();
		for (auto gameObject : view) {
			const auto& base = view.get<GameObjectBaseComponent>(gameObject);
			if (base.tag == "MainCam" && !view.get<CameraComponent>(gameObject).settings.isRenderTarget) {
				return GameObject{ gameObject,this };
			}
		}
		return GameObject::null;
	}

	GameObject Scene::getMainLight() {
		auto view = m_registry.view<GameObjectBaseComponent, LightComponent>();
		GameObject otherResult = GameObject::null;
		for (auto gameObject : view) {
			const auto& base = view.get<GameObjectBaseComponent>(gameObject);
				if (view.get<LightComponent>(gameObject).lightType == LightComponent::Directional) {
					if (base.tag == "MainLight") {
						return GameObject{ gameObject,this };
					}
					else {
						otherResult = GameObject{ gameObject,this };
					}

				}
		}
		return otherResult;
	}

	void Scene::updateTransformAndChangePhysicsPositionAndDoTheSameWithAllChilds(GameObject parent) {
		auto& view = m_registry.view<TransformComponent>();
		for (auto goId : view) {
			GameObject go = GameObject(goId, this);
			TransformComponent& tr = view.get<TransformComponent>(goId);
			if (tr.parent == parent) {
				updateTransformAndChangePhysicsPositionAndDoTheSameWithAllChilds(go);
				if (go.hasComponent<RigidbodyComponent>()) {

					this->updateTransform(tr);
					go.getComponent<RigidbodyComponent>().setTransform(tr.worldPosition, tr.worldRotation);
				}

			}
		}

		{
			TransformComponent& tr = parent.getComponent<TransformComponent>();
			if (parent.hasComponent<RigidbodyComponent>()) {
				this->updateTransform(tr);
				parent.getComponent<RigidbodyComponent>().setTransform(tr.worldPosition, tr.worldRotation);
			}
		}

	}

	GameObject Scene::findGameObjectByName(const std::string& name) {
		auto view = m_registry.view<GameObjectBaseComponent>();
		for (auto gameObject : view)
		{
			if (view.get<GameObjectBaseComponent>(gameObject).name == name)
				return GameObject(gameObject, this);
		}
		return GameObject::null;
	}

	Ref<Scene> Scene::copy(Ref<Scene> scene) {
		Ref<Scene> newScene = createRef<Scene>(scene->m_data);

		newScene->m_viewportWidth = scene->m_viewportWidth;
		newScene->m_viewportHeight = scene->m_viewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;
		//gameobject with uuid of first has a parent gameobject with uuid of second
		std::unordered_map<UUID, UUID> parentMap;

		// Create entities in new scene
		auto& srcSceneRegistry = scene->m_registry;
		auto& dstSceneRegistry = newScene->m_registry;
		auto idView = srcSceneRegistry.view<GameObjectBaseComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<GameObjectBaseComponent>(e).getUUID();
			const auto& name = srcSceneRegistry.get<GameObjectBaseComponent>(e).name;
			GameObject newGameObject = newScene->createGameObject(name, uuid, (uint32_t)e);
			newGameObject.getComponent<GameObjectBaseComponent>().tag = srcSceneRegistry.get<GameObjectBaseComponent>(e).tag;
			enttMap[uuid] = newGameObject;
			if (srcSceneRegistry.get<TransformComponent>(e).parent)
				parentMap[uuid] = srcSceneRegistry.get<TransformComponent>(e).parent.getComponent<GameObjectBaseComponent>().getUUID();
		}

		Scene* temp = s_activeScene;
		s_activeScene = newScene.get();
		// Copy components (except GameObjectBaseComponent)
		for (auto& [id, func] : Component::m_componentCopyList) {
			func(dstSceneRegistry, srcSceneRegistry, enttMap);
		}
		//CopyAllComponents(dstSceneRegistry, srcSceneRegistry, enttMap);

		s_activeScene = temp;

		for (auto& [objUUID, parUUID] : parentMap) {
			GameObject::getById(objUUID, newScene.get()).getComponent<TransformComponent>().parent = GameObject::getById(parUUID, newScene.get());
		}

		return newScene;
	}
}
