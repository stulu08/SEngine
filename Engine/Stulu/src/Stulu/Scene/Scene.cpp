#include "st_pch.h"
#include "Scene.h"

#include "GameObject.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Input/Input.h"
#include "Stulu/Core/Time.h"
#include "Stulu/Events/Event.h"
#include "Stulu/Math/Math.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Scene/SceneCamera.h"
#include "Stulu/Scene/SceneRenderer.h"
#include "Stulu/Scripting/EventCaller.h"
#include "Stulu/Scripting/Managed/Bindings/Bindings.h"


namespace Stulu {
	Scene::Scene() {
		m_renderer = createScope<SceneRenderer>(this);
		if(Application::get().getAssemblyManager())
			m_caller = createRef<EventCaller>(this);
	}
	Scene::Scene(const SceneData data, bool createSceneCaller)
		: m_data(data) {
		m_renderer = createScope<SceneRenderer>(this);
		
		if (Application::get().getAssemblyManager() && createSceneCaller) {
			m_caller = createRef<EventCaller>(this);
		}
	}
	Scene::~Scene() {
		m_registry.clear();
	}

	GameObject Scene::createGameObject(entt::entity id) {
		return createGameObject("GameObject", id);
	}

	GameObject Scene::createGameObject(const std::string& name, entt::entity id) {
		GameObject go;
		if (id == entt::null) {
			go = { m_registry.create(), this };
		}
		else {
			go = { m_registry.create((entt::entity)id), this };
			if (go.GetID() != id) {
				CORE_WARN("Warining GameObject '{0}' could not be assigned to ID: {1}", name, id);
				CORE_WARN("Instead it was given the ID: {0}", go.GetID());
			}
		}

		auto& base = go.addComponent<GameObjectBaseComponent>(!name.empty() ? name : "GameObject");
		go.addComponent<TransformComponent>();
		m_caller->NativeGameObjectCreate(go);
		return go;
	}
	void Scene::destroyGameObject(GameObject gameObject) {
		if (!gameObject) {
			CORE_ERROR("Cant destroy GameObject null");
			return;
		}
		m_caller->NativeGameObjectDestory(gameObject);
		m_caller->onDestroy(gameObject);

		//destroy all childs
		TransformComponent& transform = gameObject.getComponent<TransformComponent>();
		for (entt::entity child : transform.GetChildren()) {
			destroyGameObject({ child, this });
		}

		m_registry.destroy(gameObject.GetID());
	}

	void Scene::onUpdateEditor(SceneCamera& camera, bool render) {
		ST_PROFILING_SCOPE("Scene - Editor Update");
		if (render) {
			renderSceneEditor(camera);
		}
	}
	void Scene::onUpdateRuntime(bool render) {
		ST_PROFILING_SCOPE("Scene - Runtime Update");
		updatesRan = false;

		// basicly the firt runtime update
		if (m_sceneRuntimeTime == 0.0f)
			m_caller->onStart();

		m_sceneRuntimeTime += Time::deltaTime;

		//calculations
		m_caller->NativePreUpdate();
		GeneralUpdates();

		//rendering
		if(render) {
			renderScene();
		}
	}
	void Scene::GeneralUpdates() {
		if (updatesRan)
			return;
		m_caller->onUpdate();
		setupSceneForRendering(true);
		updatesRan = true;
	}
	void Scene::renderScene(bool callEvents) {
		ST_PROFILING_SCOPE("Scene - Runtime Render");
		//setupSceneForRendering(callEvents); //we do it while waiting for physx
		auto& view = m_registry.view<CameraComponent>();
		for (auto goID : view) {
			renderSceneForCamera(GameObject(goID, this), callEvents);
		}
		closeSceneForRendering();
	}

	void Scene::setupSceneForRendering(bool callEvents) {
		m_renderer->Begin();

		if(callEvents) {
			m_caller->onRender();
		}
	}

	void Scene::renderSceneForCamera(GameObject go, bool callEvents) {
		CameraComponent& cameraComp = go.getComponent<CameraComponent>();
		TransformComponent& transformComp = go.getComponent<TransformComponent>();
		Renderer::uploadCameraBufferData(cameraComp.GetNativeCamera().getProjectionMatrix(), glm::inverse(transformComp.GetWorldTransform()),
			transformComp.GetWorldPosition(), transformComp.GetWorldEulerRotation(), cameraComp.GetNear(), cameraComp.GetFar());

		//clear 
		cameraComp.GetNativeCamera().bindFrameBuffer();
		m_renderer->Clear(cameraComp);
		cameraComp.GetNativeCamera().unbindFrameBuffer();

		//draw 3D
		m_renderer->DrawSceneToCamera(transformComp, cameraComp);
		//draw 2D stuff
		cameraComp.GetNativeCamera().bindFrameBuffer();
		Renderer2D::begin();
		if (callEvents) {
			m_caller->onRender2D();
		}

		m_renderer->drawAll2d(transformComp);
		Renderer2D::flush();
		cameraComp.GetNativeCamera().unbindFrameBuffer();
	}
	void Scene::closeSceneForRendering() {
		m_renderer->End();
	}

	void Scene::renderSceneEditor(SceneCamera& camera) {
		ST_PROFILING_SCOPE("Scene - Editor Render");

		m_renderer->Begin();
		GameObject mc = getMainCamera();

		Renderer::uploadCameraBufferData(camera.getCamera().getProjectionMatrix(), glm::inverse(camera.getTransform().GetWorldTransform()),
			camera.getTransform().GetWorldPosition(), camera.getTransform().GetWorldEulerRotation(), camera.getNear(), camera.getFar());
		//clear 
		camera.getCamera().bindFrameBuffer();
		if (mc)
			m_renderer->Clear(mc.getComponent<CameraComponent>());
		else
			m_renderer->Clear();

		//only in editor
		if ((m_data.shaderFlags & ST_ShaderViewFlags_DisplayVertices))
			RenderCommand::setWireFrame(true);

		camera.getCamera().unbindFrameBuffer();

		//draw 3D
		if (mc)
			m_renderer->DrawSceneToCamera(camera, mc.getComponent<CameraComponent>());
		else
			m_renderer->DrawSceneToCamera(camera);

		//draw 2D stuff
		camera.getCamera().bindFrameBuffer();
		Renderer2D::begin();
		m_renderer->drawAll2d(camera.getTransform());
		Renderer2D::flush();
		camera.getCamera().unbindFrameBuffer();

		//only in editor
		if ((m_data.shaderFlags & ST_ShaderViewFlags_DisplayVertices))
			RenderCommand::setWireFrame(false);

		m_renderer->End();
		m_renderer->ApplyPostProcessing(camera);
	}

	void Scene::onRuntimeStart() {
		m_caller->NativeSceneStart();
		m_caller->ConstructManaged();

		for (entt::entity goID : getAllGameObjectsWith<MeshRendererComponent>()) {
			if (m_registry.get<MeshRendererComponent>(goID).material)
				m_registry.get<MeshRendererComponent>(goID).material->uploadData();
		}
		m_sceneRuntimeTime = 0.0f;
	}
	void Scene::onRuntimeStop() {
		m_caller->onSceneExit();
		m_caller->onDestroy();

		m_sceneRuntimeTime = 0.0f;
	}

	GameObject Scene::createEmptyGameObject(entt::entity id) {
		GameObject go;
		if (id == entt::null) {
			go = { m_registry.create(), this };
		}
		else {
			go = { m_registry.create((entt::entity)id), this };
			if (go.GetID() != id) {
				CORE_WARN("Warining Internal GameObject could not be assigned to ID: {0}", id);
				CORE_WARN("Instead it was given the ID: {0}", go.GetID());
			}
		}
		return go;
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height) {
		m_viewportWidth = width;
		m_viewportHeight = height;
		auto view = m_registry.view<CameraComponent>();
		for (auto entity : view) {
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if(!cameraComponent.IsRenderTarget())
				cameraComponent.ResizeTexture(width, height);
		}
	}

	void Scene::onEvent(Event& e) {
		
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
			root.getComponent<TransformComponent>().SetParent(GameObject::null);
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
				go.getComponent<TransformComponent>().AddChild(c);
			}
				
		}
		if (mesh.parentMeshAsset == UUID::null)
			go.getComponent<TransformComponent>().SetParent(GameObject::null);
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
			if (base.tag == "MainCam" && !view.get<CameraComponent>(gameObject).IsRenderTarget()) {
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
		Ref<Scene> newScene = createRef<Scene>(scene->m_data, false);

		newScene->m_viewportWidth = scene->m_viewportWidth;
		newScene->m_viewportHeight = scene->m_viewportHeight;

		// copy layers
		newScene->m_caller = createRef<EventCaller>(newScene.get(), scene.get());

		// parent, childs
		std::unordered_map<entt::entity, std::vector<entt::entity>> parentMap;

		// Create entities in new scene
		auto& srcSceneRegistry = scene->m_registry;
		auto& dstSceneRegistry = newScene->m_registry;
		auto idView = srcSceneRegistry.view<GameObjectBaseComponent>();
		for (entt::entity e : idView) {
			GameObject newGameObject = newScene->createEmptyGameObject(e);
			// other wise cant use the copy constructor to make them dirty
			newGameObject.addComponent<TransformComponent>();
		}

		StuluBindings::SetCurrentScene(newScene.get());

		// Copy components (except GameObjectBaseComponent)
		for (auto& [id, func] : Component::m_componentCopyList) {
			func(dstSceneRegistry, srcSceneRegistry, newScene.get());
		}
		//CopyAllComponents(dstSceneRegistry, srcSceneRegistry, enttMap);


		return newScene;
	}
}
