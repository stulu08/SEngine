#pragma once
#include "Stulu/Scene/Components.h"
#include "Stulu/Renderer/Renderer.h"
#include "SceneCamera.h"
namespace Stulu {
	class STULU_API SceneRenderer {
	public:
		static void init(Scene* scene);

		static void beginScene(GameObject object);
		static void beginScene(const SceneCamera& cam);
		static void endScene();

		static void calculateLights();
		static void uploadBuffers(const SceneData& data);

		static void submit(MeshRendererComponent& mesh, MeshFilterComponent& filter, TransformComponent& transform);

		static void drawSkyBox(SkyBoxComponent& skybox);
	private:
		static struct RuntimeData {
			struct LightData {
				struct Light {
					glm::vec4 colorAndStrength = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
					glm::vec4 positionAndType = glm::vec4(1.0f, 5.0f, 10.0f, 0.0f);
					glm::vec4 rotation = glm::vec4(90.0f, .0f, .0f, 1.0f);
					glm::vec4 spotLightData = glm::vec4(1.0f);
				} lights[MAXLIGHTS];
				uint32_t lightCount = 0;
			} lightData;
			struct SceneData {
				float toneMappingExposure = 1.0f;
				float gamma = 2.2f;
			} bufferData;

			inline static Ref<Camera> cam = nullptr;
			inline static Ref<UniformBuffer> sceneDataBuffer = nullptr;
			inline static Ref<UniformBuffer> lightBuffer = nullptr;
		} s_runtimeData;
		static inline Scene* s_scene = nullptr;
		static inline UUID s_lastMaterial = UUID::null;
	};
}

