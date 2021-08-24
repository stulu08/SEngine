#pragma once

#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Renderer/Texture.h"
#include "Stulu/Events/ApplicationEvent.h"

namespace Stulu{
	class Renderer {

	public:
		static void init();
		static void onWinndowResize(WindowResizeEvent& e);

		static void beginScene(Camera& cam);
		static void endScene();

		static void submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, uint32_t count = 0);
		static void submit(Stulu::Mesh& mesh, const Ref<Shader>& shader, const glm::mat4& transform);

		inline static RenderAPI::API getRendererAPI() { return RenderAPI::getAPI(); }
	private:

		struct SceneData {
			glm::mat4 viewProjectionMatrix;
			glm::mat4 view;
			glm::mat4 proj;
			glm::vec3 camPos;
		};

		static Scope<SceneData> m_sceneData;
	};

}