#pragma once

#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Renderer/UniformBuffer.h"

namespace Stulu{
	struct TransformComponent;
	class Renderer {

	public:
		static void init();
		static void onWinndowResize(WindowResizeEvent& e);

		static void beginScene(const Ref<Camera>& cam, const TransformComponent& transform);
		static void endScene();

		static void submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, uint32_t count = 0);
		static void submit(Stulu::Mesh& mesh, const Ref<Shader>& shader, const glm::mat4& transform);
		static void submit(Stulu::Mesh& mesh, const Ref<Shader>& shader, glm::mat4& transform);

		inline static RenderAPI::API getRendererAPI() { return RenderAPI::getAPI(); }
	private:
		struct Data {
			struct SceneData {
				glm::mat4 viewProjectionMatrix;
				glm::mat4 viewMatrix;
				glm::mat4 projMatrix;
				glm::vec3 cameraPosition;
				glm::vec3 cameraRotation;
			};
			SceneData sceneData;
			Ref<UniformBuffer> sceneDataUniformBuffer;
			Ref<Camera> cam = nullptr;
		};
		static Data m_data;
	};

}