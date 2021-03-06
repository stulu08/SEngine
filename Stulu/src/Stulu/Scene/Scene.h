#pragma once
#include "Stulu/Renderer/UniformBuffer.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Renderer/Model.h"
#include "Stulu/Scene/physx/PhysX.h"
#include "Stulu/Core/UUID.h"
#include <entt.hpp>

namespace Stulu {
#define MAXLIGHTS 25
	struct SceneData {
		float toneMappingExposure = 1.0f;
		float gamma = 2.2f;
		bool framebuffer16bit = false;
		bool enablePhsyics3D = true;
		PhysicsData physicsData;
	};

	class STULU_API GameObject;
	class STULU_API SceneCamera;

	class STULU_API Scene {
	public:
		Scene();
		~Scene();

		GameObject createGameObject(const std::string& name = "GameObject", UUID uuid = UUID());
		void destroyGameObject(GameObject gameObject);

		void onUpdateEditor(Timestep ts, const SceneCamera& camera);
		void onRuntimeStart();
		void onUpdateRuntime(Timestep ts);
		void onRuntimeStop();

		void onViewportResize(uint32_t width, uint32_t height);

		GameObject addModel(Model& model);
		GameObject addMeshAssetsToScene(MeshAsset& mesh, Model& model);
		GameObject addMeshAssetToScene(MeshAsset& mesh);

		GameObject getMainCamera();
		SceneData& getData() { return m_data; }
	private:
		uint32_t m_viewportWidth = 1, m_viewportHeight = 1;

		entt::registry m_registry;
		SceneData m_data;

		PhysX m_physics;
		bool updatedLightsThisFrame = false;

		void setupPhysics();
		void updatePhysics();
		void calculateLights();
		void renderScene(entt::entity cam, Timestep ts);

		template<typename T>
		void onComponentAdded(GameObject gameObject, T& component);

		template<typename T>
		void onComponentRemove(GameObject gameObject, T& component);

		friend class SceneRenderer;
		friend class GameObject;
		friend class EditorLayer;
		friend class SceneSerializer;
		friend class EditorHierarchyPanel;
	};
}

