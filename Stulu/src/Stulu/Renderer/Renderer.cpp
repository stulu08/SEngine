#include "st_pch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Stulu/Renderer/Renderer2D.h"
#include "Stulu/Scene/Components.h"

namespace Stulu {

	Renderer::Data Renderer::m_runtimeData;

	void Renderer::init() {
		ST_PROFILING_FUNCTION();
		RenderCommand::init();

#if OPENGL
		Renderer2D::init();
		m_runtimeData.sceneDataUniformBuffer = UniformBuffer::create(sizeof(Data::SceneRuntimeData), 0);
#endif
	}

	void Renderer::onWindowResize(WindowResizeEvent& e) {
		ST_PROFILING_FUNCTION();
		RenderCommand::setViewport(0, 0, e.getWidth(), e.getHeight());
	}
	void Renderer::beginScene(const Ref<Camera>& cam, const glm::mat4& transform) {
		ST_PROFILING_FUNCTION();
		m_runtimeData.sceneData.viewMatrix = glm::inverse(transform);
		m_runtimeData.sceneData.projMatrix = cam->getProjectionMatrix();
		m_runtimeData.sceneData.viewProjectionMatrix = m_runtimeData.sceneData.projMatrix * m_runtimeData.sceneData.viewMatrix;
		glm::vec3 pos, rot, scale;
		Math::decomposeTransform(transform, pos, rot, scale);
		m_runtimeData.sceneData.cameraPosition = pos;
		m_runtimeData.sceneData.cameraRotation = rot;

		m_runtimeData.sceneDataUniformBuffer->setData(&m_runtimeData.sceneData, sizeof(Data::SceneRuntimeData));
	}
	void Renderer::beginScene() {
		ST_PROFILING_FUNCTION();
	}
	void Renderer::endScene() {
		ST_PROFILING_FUNCTION();
	}
	void Renderer::submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, uint32_t count) {
		ST_PROFILING_FUNCTION();
		if (shader) {
			shader->bind();
			shader->setMat4("u_transform", transform);
		}
		vertexArray->bind();
		
		RenderCommand::drawIndexed(vertexArray, count);
	}
}