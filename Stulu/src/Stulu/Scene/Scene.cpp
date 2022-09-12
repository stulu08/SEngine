#include "st_pch.h"
#include "Scene.h"

#include "Stulu/Events/Event.h"
#include "Stulu/Scene/SceneRenderer.h"
#include "Stulu/Scene/physx/PhysX.h"
#include "Stulu/Scene/SceneCamera.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Scene/Behavior.h"
#include "Stulu/Math/Math.h"
#include "Stulu/Core/Time.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Core/Input.h"
#include "Stulu/ScriptCore/AssemblyManager.h"
#include "GameObject.h"

#include "PxConfig.h"
#include "physx/PxPhysicsAPI.h"

namespace Stulu {
	Scene::Scene() {
		ST_PROFILING_FUNCTION();
		SceneRenderer::init();
		if(!s_physics.started())
			s_physics.startUp();
	}
	Scene::~Scene() {
		ST_PROFILING_FUNCTION();
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
		updateAssemblyScripts("onDestroy()");
		if (gameObject.hasComponent<NativeBehaviourComponent>()) {
			auto& b = gameObject.getComponent<NativeBehaviourComponent>();
			if (b.instance) {
				b.instance->onDestroy();
				gameObject.getComponent<NativeBehaviourComponent>().DestroyBehaviour(&b);
			}
		}

		if (m_data.enablePhsyics3D && gameObject.hasComponent<RigidbodyComponent>()) {
			gameObject.removeComponent<RigidbodyComponent>();
		}

		//destroy all childs
		m_registry.view<TransformComponent>().each([=](entt::entity go, TransformComponent& transform) {
			if (transform.parent == gameObject)
				destroyGameObject({ go, this });
			});
		m_registry.destroy(gameObject);
		gameObject = GameObject::null;
	}

	void Scene::onUpdateEditor(Timestep ts, SceneCamera& camera) {
		ST_PROFILING_FUNCTION();
		s_activeScene = this;
		updateAllTransforms();
		SceneRenderer::LoadLights(this);
		if (camera.getCamera()) {
			renderSceneEditor(camera);
		}
	}
	static bool firstRuntimeUpdate = false;
	void Scene::onUpdateRuntime(Timestep ts) {
		ST_PROFILING_FUNCTION();
		s_activeScene = this;
		Time::time += ts;
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, NativeBehaviourComponent& behaviour) {
			if (!behaviour.instance) {
				behaviour.instance = behaviour.InstantiateBehaviour();
				behaviour.instance->gameObject = GameObject{ gameObject, this };
				behaviour.instance->onAwake();
			}
			behaviour.instance->onUpdate();
		});
		if(firstRuntimeUpdate)
			updateAssemblyScripts("onStart()");
		updateAssemblyScripts("onUpdate()");
		//calculations
		if(m_data.enablePhsyics3D)
			updatePhysics();
		else
			SceneRenderer::LoadLights(this);//otherwise we do it in m_physics.getScene()->fetchResults()
		updateAllTransforms();

		//rendering
		{
			renderScene();
		}
		firstRuntimeUpdate = false;
	}

	void Scene::renderScene() {
		ST_PROFILING_FUNCTION();
		SceneRenderer::Begin();
		//register all objects
		{
			auto group = m_registry.view<MeshFilterComponent, TransformComponent, MeshRendererComponent>();
			for (auto gameObject : group) {
				if (group.get<MeshFilterComponent>(gameObject).mesh.mesh) {
					SceneRenderer::RegisterObject(group.get<MeshRendererComponent>(gameObject), group.get<MeshFilterComponent>(gameObject), group.get<TransformComponent>(gameObject));
				}
			}
		}
		{
			m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, NativeBehaviourComponent& behaviour) {
				if (behaviour.instance)
					behaviour.instance->onRender(Time::frameTime);
				});
			updateAssemblyScripts("onRender()");
		}

		auto& view = m_registry.view<CameraComponent>();
		for (auto goID : view) {
			GameObject go = GameObject(goID, this);
			CameraComponent& cameraComp = go.getComponent<CameraComponent>();
			TransformComponent& transformComp = go.getComponent<TransformComponent>();
			if (cameraComp.mode == CameraMode::Perspective) {
				cameraComp.frustum = VFC::createFrustum(cameraComp.settings.aspectRatio, cameraComp.settings.zNear, 
					cameraComp.settings.zFar, cameraComp.settings.fov, transformComp);
			}
			else if (cameraComp.mode == CameraMode::Orthographic) {
				cameraComp.frustum = VFC::createFrustum(cameraComp.settings.aspectRatio, cameraComp.settings.zNear, 
					cameraComp.settings.zFar, 0, transformComp);
			}
			Renderer::uploadBufferData(cameraComp.getNativeCamera()->getProjectionMatrix(), glm::inverse(transformComp.transform), 
				transformComp.worldPosition, transformComp.eulerAnglesDegrees, cameraComp.settings.zNear, cameraComp.settings.zFar);
			
			//clear 
			cameraComp.getNativeCamera()->bindFrameBuffer();
			SceneRenderer::Clear(cameraComp);
			cameraComp.getNativeCamera()->unbindFrameBuffer();

			//draw 3D
			SceneRenderer::DrawSceneToCamera(transformComp, cameraComp, true);
			//draw 2D stuff
			cameraComp.getNativeCamera()->bindFrameBuffer();
			Renderer2D::begin();
			{
				m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, NativeBehaviourComponent& behaviour) {
					if (behaviour.instance)
						behaviour.instance->onRender2D(Time::frameTime);
					});
				updateAssemblyScripts("onRender2D()");
			}
			auto view = m_registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto gameObject : view)
			{
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(gameObject);
				if (sprite.texture)
					Renderer2D::drawTexturedQuad(transform, sprite.texture, sprite.texture->getSettings().tiling * sprite.tiling, sprite.color);
				else
					Renderer2D::drawQuad(transform, sprite.color);
			}
			auto circleView = m_registry.view<TransformComponent, CircleRendererComponent>();
			for (auto gameObject : circleView)
			{
				auto [transform, sprite] = circleView.get<TransformComponent, CircleRendererComponent>(gameObject);
				Renderer2D::drawCircle(transform, sprite.color, sprite.thickness, sprite.fade);
			}
			Renderer2D::flush();
			cameraComp.getNativeCamera()->unbindFrameBuffer();

		}
		SceneRenderer::End();
	}

	void Scene::renderSceneEditor(SceneCamera& camera) {
		ST_PROFILING_FUNCTION();
		SceneRenderer::Begin();
		{
			auto group = m_registry.view<MeshFilterComponent, TransformComponent, MeshRendererComponent>();
			for (auto gameObject : group) {
				if (group.get<MeshFilterComponent>(gameObject).mesh.mesh) {
					SceneRenderer::RegisterObject(group.get<MeshRendererComponent>(gameObject), group.get<MeshFilterComponent>(gameObject), group.get<TransformComponent>(gameObject));
				}
			}
		}
		GameObject mc = getMainCamera();

		Renderer::uploadBufferData(camera.getCamera()->getProjectionMatrix(), glm::inverse(camera.getTransform().transform),
			camera.getTransform().worldPosition, camera.getTransform().eulerAnglesDegrees, camera.getNear(), camera.getFar());
		//clear 
		camera.getCamera()->bindFrameBuffer();
		if (mc) {
			SceneRenderer::Clear(mc.getComponent<CameraComponent>());
			//draw sky
			SkyBoxComponent sbC;
			if (mc.saveGetComponent<SkyBoxComponent>(sbC)) {
				SceneRenderer::drawSkyBox(sbC.texture);
			}
		}
		else
			SceneRenderer::Clear();

		//only in editor
		if ((m_data.shaderFlags & ST_ShaderViewFlags_DisplayVertices))
			RenderCommand::setWireFrame(true);

		camera.getCamera()->unbindFrameBuffer();

		//draw 3D
		if (mc)
			SceneRenderer::DrawSceneToCamera(camera, mc.getComponent<CameraComponent>());
		else
			SceneRenderer::DrawSceneToCamera(camera, this);

		//draw 2D stuff
		camera.getCamera()->bindFrameBuffer();
		Renderer2D::begin();
		auto view = m_registry.view<TransformComponent, SpriteRendererComponent>();
		for (auto gameObject : view)
		{
			auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(gameObject);
			if (sprite.texture)
				Renderer2D::drawTexturedQuad(transform, sprite.texture, sprite.texture->getSettings().tiling * sprite.tiling, sprite.color);
			else
				Renderer2D::drawQuad(transform, sprite.color);
		}
		auto circleView = m_registry.view<TransformComponent, CircleRendererComponent>();
		for (auto gameObject : circleView)
		{
			auto [transform, sprite] = circleView.get<TransformComponent, CircleRendererComponent>(gameObject);
			Renderer2D::drawCircle(transform, sprite.color, sprite.thickness, sprite.fade);
		}
		Renderer2D::flush();
		camera.getCamera()->unbindFrameBuffer();

		//only in editor
		if ((m_data.shaderFlags & ST_ShaderViewFlags_DisplayVertices))
			RenderCommand::setWireFrame(false);

		SceneRenderer::End();
		SceneRenderer::ApplyPostProcessing(camera);
	}

	void Scene::onRuntimeStart() {
		ST_PROFILING_FUNCTION();
		s_activeScene = this;
		Time::time = .0f;
		firstRuntimeUpdate = true;
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

		updateAssemblyScripts("", true);

		m_registry.view<CameraComponent>().each([=](auto gameObject, CameraComponent& cam) {
			//cam.getFrameBuffer()->getSpecs().textureFormat = (m_data.framebuffer16bit ? TextureSettings::Format::RGBA16F : TextureSettings::Format::RGBA);
			cam.getFrameBuffer()->resize(cam.settings.textureWidth, cam.settings.textureHeight);
			});

		for (entt::entity goID : getAllGameObjectsWith<MeshRendererComponent>()) {
			if(m_registry.get<MeshRendererComponent>(goID).material)
				m_registry.get<MeshRendererComponent>(goID).material->uploadData();
		}

		Time::deltaTime = .0f;
	}
	void Scene::onRuntimeStop() {
		ST_PROFILING_FUNCTION();
		s_activeScene = this;
		m_registry.view<NativeBehaviourComponent>().each([=](auto gameObject, NativeBehaviourComponent& behaviour) {
			if (behaviour.instance) {
				behaviour.instance->onSceneExit();
				behaviour.instance->onDestroy();
				behaviour.DestroyBehaviour(&behaviour);
			}
			});
		updateAssemblyScripts("onSceneExit()");
		updateAssemblyScripts("onDestroy()");
		
		Input::setCursorMode(Input::CursorMode::Normal);
		if (m_data.enablePhsyics3D) {
			s_physics.releasePhysics();
		}
	}

	void Scene::setupPhysics() {
		ST_PROFILING_FUNCTION();
		s_physics.createPhysics(m_data.physicsData);
		for (auto id : m_registry.view<BoxColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<BoxColliderComponent>().rigidbody == nullptr)
				object.getComponent<BoxColliderComponent>().create(object, s_physics);
		}
		for (auto id : m_registry.view<SphereColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<SphereColliderComponent>().rigidbody == nullptr)
				object.getComponent<SphereColliderComponent>().create(object, s_physics);
		}
		for (auto id : m_registry.view<CapsuleColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<CapsuleColliderComponent>().rigidbody == nullptr)
				object.getComponent<CapsuleColliderComponent>().create(object, s_physics);
		}
		for (auto id : m_registry.view<MeshColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<MeshColliderComponent>().rigidbody == nullptr)
				object.getComponent<MeshColliderComponent>().create(object, s_physics);
		}
	}
	void Scene::updatePhysics() {
		ST_PROFILING_FUNCTION();
		if (Time::deltaTime == 0.0f || firstRuntimeUpdate) {
			return;
		}
		s_physics.getScene()->simulate(Time::deltaTime);
		//we do as much as possible here
		while (!s_physics.getScene()->fetchResults()) {
			SceneRenderer::LoadLights(this);
		}
		for (auto id : m_registry.view<BoxColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<BoxColliderComponent>().rigidbody == nullptr)
				object.getComponent<BoxColliderComponent>().create(object, s_physics);
		}
		for (auto id : m_registry.view<SphereColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<SphereColliderComponent>().rigidbody == nullptr)
				object.getComponent<SphereColliderComponent>().create(object, s_physics);
		}
		for (auto id : m_registry.view<CapsuleColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<CapsuleColliderComponent>().rigidbody == nullptr)
				object.getComponent<CapsuleColliderComponent>().create(object, s_physics);
		}
		for (auto id : m_registry.view<MeshColliderComponent>()) {
			GameObject object = { id, this };
			if (object.getComponent<MeshColliderComponent>().rigidbody == nullptr)
				object.getComponent<MeshColliderComponent>().create(object, s_physics);
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
		ST_PROFILING_FUNCTION();
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
		ST_PROFILING_FUNCTION();
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
		ST_PROFILING_FUNCTION();
		if (e.getEventType() == EventType::WindowClose) {
			if (s_physics.started())
				s_physics.shutDown();
		}
		
	}

	

	bool Scene::initlizeGameObjectAttachedClass(entt::entity gameObject, Ref<MonoObjectInstance>& i) {
		ST_PROFILING_FUNCTION();
		Ref<ScriptAssembly> assembly = Application::get().getAssemblyManager()->getScriptCoreAssembly();
		MonoClass* goAttachClass = mono_class_from_name(assembly->getImage(), "Stulu", "GameObjectAttached");
		if (goAttachClass) {
			MonoMethod* func = i->getVirtualFunction("initilize", goAttachClass);
			if (func) {
				void* args[1];
				args[0] = &gameObject;
				assembly->invokeFunction(func, i->getObjectPtr(), args);
				return true;
			}
		}
		return false;
	}

	void Scene::updateAssemblyScripts(const std::string& function, bool forceConstructNew) {
		ST_PROFILING_FUNCTION();
		try {
			m_registry.view<ScriptingComponent>().each([=](entt::entity gameObject, ScriptingComponent& comp) {
				for (Ref<Stulu::MonoObjectInstance> i : comp.runtimeScripts) {
					if (!i->isContructed() || forceConstructNew) {
						//i->loadAllClassFunctions();
						//i->loadAllVirtualParentFunctions();
						i->callDefaultConstructor();
						i->setAllClassFields();
						//initilize component
						if (initlizeGameObjectAttachedClass(gameObject, i))
							i->call("onAwake()");


					}
					if (!function.empty())
						i->call(function);
				}
				});
		}
		catch (std::exception& ex) {
			CORE_ERROR(ex.what());
		}
		
	}

	GameObject Scene::addModel(Model& model) {
		ST_PROFILING_FUNCTION();
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
		ST_PROFILING_FUNCTION();
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
		ST_PROFILING_FUNCTION();
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
		ST_PROFILING_FUNCTION();
		auto view = m_registry.view<GameObjectBaseComponent, CameraComponent>();
		for (auto gameObject : view) {
			const auto& base = view.get<GameObjectBaseComponent>(gameObject);
			if (base.tag == "MainCam" && !view.get<CameraComponent>(gameObject).settings.isRenderTarget) {
				return GameObject{ gameObject,this };
			}
		}
		return GameObject::null;
	}

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap) {
		([&]()
			{
				auto view = src.view<Component>();
				for (auto srcGameObject : view)
				{
					entt::entity dstGameObject = enttMap.at(src.get<GameObjectBaseComponent>(srcGameObject).uuid);

					auto& srcComponent = src.get<Component>(srcGameObject);
					dst.emplace_or_replace<Component>(dstGameObject, srcComponent);
					dst.get<Component>(dstGameObject).gameObject = GameObject(dstGameObject, Scene::activeScene());
				}
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap) {
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(GameObject dst, GameObject src) {
		([&]()
			{
				if (src.hasComponent<Component>())
					dst.addOrReplaceComponent<Component>(src.getComponent<Component>());
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, GameObject dst, GameObject src) {
		CopyComponentIfExists<Component...>(dst, src);
	}

	static void CopyAllComponents(entt::registry& dstSceneRegistry, entt::registry& srcSceneRegistry, const std::unordered_map<UUID, entt::entity>& enttMap) {
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);
	}

	static void CopyAllExistingComponents(GameObject dst, GameObject src) {
		CopyComponentIfExists(AllComponents{}, dst, src);
	}

	void Scene::updateTransformAndChangePhysicsPositionAndDoTheSameWithAllChilds(GameObject parent) {
		ST_PROFILING_FUNCTION();
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
		ST_PROFILING_FUNCTION();
		auto view = m_registry.view<GameObjectBaseComponent>();
		for (auto gameObject : view)
		{
			if (view.get<GameObjectBaseComponent>(gameObject).name == name)
				return GameObject(gameObject, this);
		}
		return GameObject::null;
	}

	Ref<Scene> Scene::copy(Ref<Scene> scene) {
		ST_PROFILING_FUNCTION();
		Ref<Scene> newScene = createRef<Scene>();

		newScene->m_viewportWidth = scene->m_viewportWidth;
		newScene->m_viewportHeight = scene->m_viewportHeight;
		newScene->m_data = scene->m_data;

		std::unordered_map<UUID, entt::entity> enttMap;
		//gameobject with uuid of first has a parent gameobject with uuid of second
		std::unordered_map<UUID, UUID> parentMap;

		// Create entities in new scene
		auto& srcSceneRegistry = scene->m_registry;
		auto& dstSceneRegistry = newScene->m_registry;
		auto idView = srcSceneRegistry.view<GameObjectBaseComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<GameObjectBaseComponent>(e).uuid;
			const auto& name = srcSceneRegistry.get<GameObjectBaseComponent>(e).name;
			GameObject newGameObject = newScene->createGameObject(name, uuid);
			newGameObject.getComponent<GameObjectBaseComponent>().tag = srcSceneRegistry.get<GameObjectBaseComponent>(e).tag;
			enttMap[uuid] = newGameObject;
			if (srcSceneRegistry.get<TransformComponent>(e).parent)
				parentMap[uuid] = srcSceneRegistry.get<TransformComponent>(e).parent.getComponent<GameObjectBaseComponent>().uuid;
		}
		Scene* temp = s_activeScene;
		s_activeScene = newScene.get();
		// Copy components (except GameObjectBaseComponent)
		CopyAllComponents(dstSceneRegistry, srcSceneRegistry, enttMap);
		s_activeScene = temp;


		for (auto& [objUUID, parUUID] : parentMap) {
			GameObject::getById(objUUID, newScene.get()).getComponent<TransformComponent>().parent = GameObject::getById(parUUID, newScene.get());
		}
		//copy scripts
		//for (auto entID : scene->getAllGameObjectsWith<ScriptingComponent>()) {
		//	GameObject src = GameObject(entID, scene.get());
		//	GameObject dst = GameObject::getById(src.getId(), newScene.get());
		//
		//	dst.getComponent<ScriptingComponent>().copyScriptsFrom(src.getComponent<ScriptingComponent>());
		//}

		return newScene;
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
	void STULU_API Scene::onComponentAdded<CircleRendererComponent>(GameObject gameObject, CircleRendererComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<RigidbodyComponent>(GameObject gameObject, RigidbodyComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<CharacterController>(GameObject gameObject, CharacterController& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<BoxColliderComponent>(GameObject gameObject, BoxColliderComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<SphereColliderComponent>(GameObject gameObject, SphereColliderComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<CapsuleColliderComponent>(GameObject gameObject, CapsuleColliderComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<MeshColliderComponent>(GameObject gameObject, MeshColliderComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<NativeBehaviourComponent>(GameObject gameObject, NativeBehaviourComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<ScriptingComponent>(GameObject gameObject, ScriptingComponent& component) { }
	template<>
	void STULU_API Scene::onComponentAdded<PostProcessingComponent>(GameObject gameObject, PostProcessingComponent& component) { }
}
