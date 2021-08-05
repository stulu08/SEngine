#pragma once

#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/Shader.h"
#include <Stulu/Events/ApplicationEvent.h>

namespace Stulu{
	class Renderer {

	public:
		static void init();
		static void onWinndowResize(WindowResizeEvent& e);

		static void beginScene(OrthographicCamera& cam);
		static void endScene();

		static void submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform);

		inline static RenderAPI::API getRendererAPI() { return RenderAPI::getAPI(); }
	private:
		struct SceneData {
			glm::mat4 viewProjectionMatrix;
		};

		static Scope<SceneData> m_sceneData;
	};

}