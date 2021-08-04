#include "st_pch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Stulu {
	Renderer::SceneData* Renderer::m_sceneData = new Renderer::SceneData;

	void Renderer::init() {
		RenderCommand::init();
	}

	void Renderer::beginScene(OrthographicCamera& cam) {
		m_sceneData->viewProjectionMatrix = cam.getViewProjectionMatrix();
	}
	void Renderer::endScene() {

	}
	void Renderer::submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform) {
		shader->bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadMat4Uniform("u_viewProjection", m_sceneData->viewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadMat4Uniform("u_transform", transform);
		vertexArray->bind();
		RenderCommand::drawIndex(vertexArray);
	}
}