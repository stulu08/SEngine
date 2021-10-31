#pragma once
#include "Stulu/Scene/Components.h"
#include "SceneCamera.h"
//texture slots
//0 = empty
//1 = currentSkyBox
//2-end = material
namespace Stulu {
	class SceneRenderer {
	public:
		static void init();

		static void beginScene(GameObject object);
		static void beginScene(const SceneCamera& cam);
		static void endScene();

		static void submit(MeshRendererComponent& mesh, MeshFilterComponent& filter, TransformComponent& transform);

		static void drawSkyBox(SkyBoxComponent& skybox);
	private:
		struct Data {
			struct SceneRuntimeData {
				glm::mat4 viewProjectionMatrix;
				glm::mat4 viewMatrix;
				glm::mat4 projMatrix;
				glm::vec3 cameraPosition;
				glm::vec3 cameraRotation;
			};
			SceneRuntimeData sceneData;
			Ref<UniformBuffer> sceneDataUniformBuffer = nullptr;
			UUID defaultMaterial;
			Ref<Camera> cam = nullptr;
		};
		static Data m_runtimeData;
	};
}

