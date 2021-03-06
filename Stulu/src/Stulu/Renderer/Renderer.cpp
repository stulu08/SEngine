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
		m_runtimeData.matricesDataUniformBuffer = UniformBuffer::create(sizeof(Renderer::MatricesBufferData), 0);
#endif
	}

	void Renderer::onWindowResize(WindowResizeEvent& e) {
		ST_PROFILING_FUNCTION();
		RenderCommand::setViewport(0, 0, e.getWidth(), e.getHeight());
	}
	void Renderer::beginScene(const glm::mat4& projection, const glm::mat4& transform) {
		ST_PROFILING_FUNCTION();
		uploadBufferData(projection, transform);
		Renderer2D::beginScene();
	}
	void Renderer::beginScene(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position, const glm::vec3& rotation) {
		ST_PROFILING_FUNCTION();
		uploadBufferData(projection, view, position, rotation);
		Renderer2D::beginScene();
	}
	void Renderer::endScene() {
		ST_PROFILING_FUNCTION();
		Renderer2D::endScene();
	}
	void Renderer::submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, uint32_t count) {
		ST_PROFILING_FUNCTION();
		if (shader) {
			shader->bind();
			m_runtimeData.matricesDataUniformBuffer->setData(&transform, sizeof(glm::mat4), sizeof(Renderer::MatricesBufferData) - sizeof(glm::mat4));
		}
		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray, count);
	}
	void Renderer::submit(const Ref<VertexArray>& vertexArray, const glm::mat4& transform, uint32_t count) {
		ST_PROFILING_FUNCTION();
		m_runtimeData.matricesDataUniformBuffer->setData(&transform, sizeof(glm::mat4), sizeof(Renderer::MatricesBufferData) - sizeof(glm::mat4));
		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray, count);
	}
	void Renderer::uploadBufferData(const glm::mat4& projection, const glm::mat4& transform) {
		ST_PROFILING_FUNCTION();
		glm::vec3 pos, rot, scale;
		Math::decomposeTransform(transform, pos, rot, scale);
		uploadBufferData(projection, glm::inverse(transform), pos, rot);
	}
	void Renderer::uploadBufferData(const glm::mat4& projection, const glm::mat4& view, const glm::vec3 position, const glm::vec3 rotation) {
		ST_PROFILING_FUNCTION();
		m_runtimeData.matricesData.viewMatrix = view;
		m_runtimeData.matricesData.projMatrix = projection;
		m_runtimeData.matricesData.viewProjectionMatrix = m_runtimeData.matricesData.projMatrix * m_runtimeData.matricesData.viewMatrix;
		m_runtimeData.matricesData.cameraPosition = glm::vec4(position,1.0f);
		m_runtimeData.matricesData.cameraRotation = glm::vec4(rotation, 1.0f);
		m_runtimeData.matricesData.modelMatrix = glm::mat4(1.0f);
		m_runtimeData.matricesDataUniformBuffer->setData(&m_runtimeData.matricesData, sizeof(Renderer::MatricesBufferData));
	}
}