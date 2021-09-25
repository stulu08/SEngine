#include "st_pch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Stulu/Renderer/Renderer2D.h"
#include "Stulu/Scene/Components.h"

namespace Stulu {

	Renderer::Data Renderer::m_data;

	void Renderer::init() {
		ST_PROFILING_FUNCTION();
		RenderCommand::init();
		Renderer2D::init();

		m_data.sceneDataUniformBuffer = UniformBuffer::create(sizeof(Data::SceneData), 0);
	}

	void Renderer::onWinndowResize(WindowResizeEvent& e)
	{
		ST_PROFILING_FUNCTION();
		RenderCommand::setViewport(0, 0, e.getWidth(), e.getHeight());
	}
	void Renderer::beginScene(const Ref<Camera>& cam, const TransformComponent& transform) {
		ST_PROFILING_FUNCTION();
		m_data.sceneData.viewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), transform.position) * glm::toMat4(glm::quat(glm::radians(transform.rotation))));
		m_data.sceneData.projMatrix = cam->getProjectionMatrix();
		m_data.sceneData.viewProjectionMatrix = m_data.sceneData.projMatrix * m_data.sceneData.viewMatrix;
		m_data.sceneData.cameraPosition = transform.position;
		m_data.sceneData.cameraRotation = transform.rotation;

		m_data.sceneDataUniformBuffer->setData(&m_data.sceneData, sizeof(Data::SceneData));
	}
	void Renderer::endScene() {

		ST_PROFILING_FUNCTION();

	}
	void Renderer::submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, uint32_t count) {
		ST_PROFILING_FUNCTION();
		shader->bind();
		shader->setMat4("u_transform", transform);
		vertexArray->bind();
		
		RenderCommand::drawIndexed(vertexArray, count);
	}
	void Renderer::submit(Mesh& mesh, const Ref<Shader>& shader, const glm::mat4& transform) {
		submit(mesh.getVertexArray(), shader, transform);
	}
}