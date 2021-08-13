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
		static void setUpSkyBox();
		static void onWinndowResize(WindowResizeEvent& e);

		static void beginScene(Camera& cam, bool skybox = false);
		static void endScene();

		static void setSkyBox(Ref<CubeMap> texture) {
			m_sceneData->skybox.texture = texture; }

		static void submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform);
		static void submit(Stulu::Mesh& mesh, const Ref<Shader>& shader, const glm::mat4& transform);

		inline static RenderAPI::API getRendererAPI() { return RenderAPI::getAPI(); }
	private:
		struct SkyBox {
			Ref<VertexArray> vertexArray;
			Ref<Shader> shader;
			Ref<CubeMap> texture;
			uint32_t count = 36;
		};
		struct SceneData {
			glm::mat4 viewProjectionMatrix;
			glm::mat4 view;
			glm::mat4 proj;
			glm::vec3 camPos;
			bool skyBoxClear;
			SkyBox skybox;
		};

		static Scope<SceneData> m_sceneData;
	};

}