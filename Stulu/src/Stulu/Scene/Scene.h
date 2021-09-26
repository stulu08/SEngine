#pragma once
#include "Stulu/Renderer/UniformBuffer.h"
#include "Stulu/Scene/SceneCamera.h"
#include "Stulu/Scene/Components.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Core/Timestep.h"

#include <entt.hpp>

namespace Stulu {
#define MAXLIGHTS 25
	struct SceneData {
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
		Ref<UniformBuffer> lightBuffer;
	};

	class GameObject;

	class Scene {
	public:
		Scene();
		~Scene();

		GameObject createGameObject(const std::string& name = "GameObject");
		void destroyGameObject(GameObject& gameObject);

		void onUpdateEditor(Timestep ts, const SceneCamera& camera);
		void onUpdateRuntime(Timestep ts);

		void onViewportResize(uint32_t width, uint32_t height);

		GameObject getMainCamera();
	private:
		uint32_t m_viewportWidth, m_viewportHeight;

		entt::registry m_registry;
		SceneData m_data;

		void calculateLights();

		template<typename T>
		void onComponentAdded(GameObject gameObject, T& component);

		friend class GameObject;
		friend class EditorHierarchyPanel;
	};
}

