#include "st_pch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Stulu/Renderer/Renderer2D.h"

namespace Stulu {
	Scope<Renderer::SceneData> Renderer::m_sceneData = createScope<Renderer::SceneData>();

	void Renderer::init() {
		ST_PROFILING_FUNCTION();
		RenderCommand::init();
		Renderer2D::init();
	}


	void Renderer::onWinndowResize(WindowResizeEvent& e)
	{
		ST_PROFILING_FUNCTION();
		RenderCommand::setViewport(0, 0, e.getWidth(), e.getHeight());
	}

	void Renderer::beginScene(Camera& cam) {
		ST_PROFILING_FUNCTION();
		m_sceneData->viewProjectionMatrix = cam.getViewProjectionMatrix();
		m_sceneData->view = cam.getViewMatrix();
		m_sceneData->proj = cam.getProjectionMatrix();
		m_sceneData->camPos = cam.getPos();
	}
	void Renderer::endScene() {

		ST_PROFILING_FUNCTION();

	}
	void Renderer::submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, uint32_t count) {
		ST_PROFILING_FUNCTION();
		shader->bind();
		shader->setMat4("u_viewProjection", m_sceneData->viewProjectionMatrix);
		shader->setMat4("u_transform", transform);
		shader->setFloat3("u_camPos", m_sceneData->camPos);
		vertexArray->bind();
		
		RenderCommand::drawIndexed(vertexArray, count);
	}
	void Renderer::submit(Mesh& mesh, const Ref<Shader>& shader, const glm::mat4& transform) {
		submit(mesh.getVertexArray(), shader, transform);
	}
}