#pragma once
#include "Stulu/Renderer/UniformBuffer.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Scene/physx/PhysX.h"
#include "Stulu/Core/UUID.h"
#include <entt.hpp>

namespace Stulu {
#define MAXLIGHTS 25
	struct SceneData {
		PhysicsData physicsData;
		bool enablePhsyics3D = true;
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
		void onUpdateRuntime(Timestep ts);

		void onRuntimeStart();
		void onRuntimeStop();
		void onViewportResize(uint32_t width, uint32_t height);

		GameObject getMainCamera();
		SceneData& getData() { return m_data; }
	private:
		uint32_t m_viewportWidth = 1, m_viewportHeight = 1;

		entt::registry m_registry;
		SceneData m_data;

		struct SceneRuntimeData {
			struct LightData {
				struct Light {
					glm::vec4 colorAndStrength = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
					glm::vec4 positionAndType = glm::vec4(1.0f, 5.0f, 10.0f, 0.0f);
					glm::vec4 rotation = glm::vec4(90.0f, .0f, .0f, 1.0f);
					glm::vec4 spotLightData = glm::vec4(1.0f);
				};
				Light lights[MAXLIGHTS];
				uint32_t lightCount = 0;
			};
			LightData lightData;
		} m_runtimeData;

		PhysX m_physics;
		bool updatedLightsThisFrame = false;

		void setupPhysics();

		void updatePhysics();
		void calculateLights();
		void renderScene(entt::entity cam);

		template<typename T>
		void onComponentAdded(GameObject gameObject, T& component);

		template<typename T>
		void onComponentRemove(GameObject gameObject, T& component);

		friend class GameObject;
		friend class EditorLayer;
		friend class SceneSerializer;
		friend class EditorHierarchyPanel;


		static Ref<UniformBuffer> s_lightBuffer;
	};
}

