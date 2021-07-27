#pragma once

#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/Shader.h"

namespace Stulu{
	class Renderer {

	public:
		static void beginScene(OrthographicCamera& cam);
		static void endScene();

		static void submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader);

		inline static RenderAPI::API getRendererAPI() { return RenderAPI::getAPI(); }
	private:
		struct SceneData {
			glm::mat4 viewProjectionMatrix;
		};

		static SceneData* m_sceneData;
	};

}