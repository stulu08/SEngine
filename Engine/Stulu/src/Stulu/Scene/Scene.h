#pragma once
#include "Stulu/Renderer/Buffer.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Scene/Model.h"
#include "Stulu/Scene/physx/PhysX.h"
#include "Stulu/Core/UUID.h"
#include "Stulu/Events/MouseEvent.h"
#include "Stulu/Events/KeyEvent.h" 
#include "physx/PhysX.h"
#include <entt.hpp>

namespace Stulu {	
	struct SceneGraphicsData {
		float env_lod = 1.0f;
		float shadowDistance = 50.0f;
		float shadowFar = 500.0f;
		uint32_t shadowMapSize = 2048;
	};
	struct SceneData {
		SceneGraphicsData graphicsData;
		bool enablePhsyics3D = true;
		PhysicsData physicsData = PhysicsData();
		uint32_t shaderFlags = 0;
	};

	class GameObject;
	class STULU_API SceneCamera;
	class STULU_API MonoObjectInstance;
	class STULU_API SceneRenderer;
	class STULU_API EventCaller;

	class STULU_API Scene {
	public:
		Scene();
		Scene(const SceneData data);
		~Scene();

		GameObject createGameObject(entt::entity id = entt::null);
		GameObject createGameObject(const std::string& name = "GameObject", entt::entity id = entt::null);
		void destroyGameObject(GameObject gameObject);

		void onUpdateEditor(SceneCamera& camera, bool render = true);
		void onRuntimeStart();
		void onUpdateRuntime(bool render = true);
		void onRuntimeStop();

		void onViewportResize(uint32_t width, uint32_t height);
		void onEvent(Stulu::Event& e);

		GameObject addModel(Model& model);
		GameObject addMeshAssetsToScene(MeshAsset& mesh, Model& model);
		GameObject addMeshAssetToScene(MeshAsset& mesh);

		GameObject getMainCamera();
		GameObject getMainLight();
		inline SceneData& getData() { return m_data; }
		inline Ref<EventCaller>& getCaller() { return m_caller; }

		bool PhysicsEnable() const { return m_data.enablePhsyics3D && m_physics.get(); }
		PhysX* getPhysics() const { return m_physics.get(); }
		SceneRenderer* getRenderer() const { return m_renderer.get(); }
		int32_t getViewportWidth() const { return m_viewportWidth; }
		int32_t getViewportHeight() const { return m_viewportHeight; }
		float GetSceneRuntime() const { return m_sceneRuntimeTime; }

		void GeneralUpdates();
		GameObject findGameObjectByName(const std::string& name);

		template<typename... Components>
		inline auto getAllGameObjectsWith() {
			return m_registry.view<Components...>();
		}
		template<typename... Components>
		inline auto getAllGameObjectsAsGroupWith() {
			return m_registry.group<Components...>();
		}
		inline entt::registry& getRegistry() { return m_registry; }

		static Ref<Scene> copy(Ref<Scene> scene);

		//sets the scene up, renderes for every cam, closes for renderering
		void renderScene(bool callEvents = true);
		void setupSceneForRendering(bool callEvents = true);
		//needs setup and closing
		void renderSceneForCamera(GameObject gameObject, bool callEvents = true);
		void closeSceneForRendering();

	private:
		uint32_t m_viewportWidth = 1, m_viewportHeight = 1;
		SceneData m_data;
		Scope<PhysX> m_physics = nullptr;
		Scope<SceneRenderer> m_renderer = nullptr;
		Ref<EventCaller> m_caller = nullptr;
		float m_sceneRuntimeTime = 0.0f;

		entt::registry m_registry;

		// only for internal usage
		GameObject createEmptyGameObject(entt::entity id = entt::null);

		void setupPhysics();
		void updatePhysics();
		void createPhysicsObjects();

		void renderSceneEditor(SceneCamera& camera);

		template<typename T>
		void onComponentAdded(GameObject gameObject, T& component) {
			component.gameObject = gameObject;
			component.onComponentAdded(this);
		}
		template<typename T>
		void onComponentRemove(GameObject gameObject, T& component) {
			component.gameObject = gameObject;
			component.onComponentRemove(this);
		}

		friend class GameObject;
	};
}

