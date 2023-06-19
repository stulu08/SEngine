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
#include <entt.hpp>

namespace Stulu {
	struct SceneGraphicsData {
		float env_lod = 1.0f;
		float sampleScale = 1.7f;
		float shadowDistance = 50.0f;
		float shadowFar = 500.0f;
	};
	struct SceneData {
		SceneGraphicsData graphicsData;
		bool enablePhsyics3D = true;
		PhysicsData physicsData = PhysicsData();
		uint32_t shaderFlags = 0;
	};

	class STULU_API SceneCamera;
	class STULU_API GameObject;
	class STULU_API MonoObjectInstance;
	class STULU_API SceneRenderer;

	class STULU_API Scene {
	public:
		Scene();
		~Scene();

		GameObject createGameObject(UUID uuid);
		GameObject createGameObject(const std::string& name = "GameObject", UUID uuid = UUID());
		void destroyGameObject(GameObject gameObject);

		void onUpdateEditor(Timestep ts, SceneCamera& camera, bool render = true);
		void onRuntimeStart();
		void onUpdateRuntime(Timestep ts, bool render = true);
		void onRuntimeStop();

		void onViewportResize(uint32_t width, uint32_t height);
		void onEvent(Stulu::Event& e);

		GameObject addModel(Model& model);
		GameObject addMeshAssetsToScene(MeshAsset& mesh, Model& model);
		GameObject addMeshAssetToScene(MeshAsset& mesh);

		GameObject getMainCamera();
		GameObject getMainLight();
		SceneData& getData() { return m_data; }

		bool initlizeGameObjectAttachedClass(entt::entity gameObject, Ref<MonoObjectInstance>& instance);

		static inline void setActiveScene(Scene* scne) { s_activeScene = scne; }

		static inline Scene* activeScene() { return s_activeScene; }

		const Scope<PhysX>& getPhysics() const { return m_physics; }
		const Scope<SceneRenderer>& getRenderer() const { return m_renderer; }
		int32_t getViewportWidth() const { return m_viewportWidth; }
		int32_t getViewportHeight() const { return m_viewportHeight; }

		void runtime_updatesetups();
		void updateTransform(TransformComponent& tc);
		void updateAllTransforms();
		void updateParticles(bool render, bool update);
		void clearAllParticles();
		void updateTransformAndChangePhysicsPositionAndDoTheSameWithAllChilds(GameObject parent);
		GameObject findGameObjectByName(const std::string& name);


		template<typename... Components>
		auto getAllGameObjectsWith() {
			return m_registry.view<Components...>();
		}
		template<typename... Components>
		auto getAllGameObjectsAsGroupWith() {
			return m_registry.group<Components...>();
		}

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
		bool m_firstRuntimeUpdate = false;

		entt::registry m_registry;

		void setupPhysics();
		void updatePhysics();
		void renderSceneEditor(SceneCamera& camera);

		void updateAssemblyScripts(const std::string& function, bool forceConstructNew = false);

		template<typename T>
		void onComponentAdded(GameObject gameObject, T& component) {
			component.gameObject = gameObject;
			component.onComponentAdded(this);
		}

		template<typename T>
		void onComponentRemove(GameObject gameObject, T& component) {
			component.destroy();
		}

		friend class SceneRenderer;
		friend class GameObject;
		friend class EditorLayer;
		friend class SceneSerializer;
		friend class EditorHierarchyPanel;
		friend class ComponentsRender;


		static inline Scene* s_activeScene = nullptr;
	};
}

