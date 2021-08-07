#include "st_pch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Stulu/Renderer/Renderer2D.h"

namespace Stulu {
	Scope<Renderer::SceneData> Renderer::m_sceneData = createScope<Renderer::SceneData>();

	void Renderer::init() {
		RenderCommand::init();
		Renderer2D::init();
	}

	void Renderer::onWinndowResize(WindowResizeEvent& e)
	{
		RenderCommand::setViewport(0, 0, e.getWidth(), e.getHeight());
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