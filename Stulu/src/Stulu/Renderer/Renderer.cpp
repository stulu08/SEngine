#include "st_pch.h"
#include "Renderer.h"

namespace Stulu {
	Renderer::SceneData* Renderer::m_sceneData = new Renderer::SceneData;

	void Renderer::beginScene(OrthographicCamera& cam) {
		m_sceneData->viewProjectionMatrix = cam.getViewProjectionMatrix();
	}
	void Renderer::endScene() {

	}
	void Renderer::submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader) {
		shader->bind();
		shader->uploadMat4Uniform("u_viewProjection", m_sceneData->viewProjectionMatrix);
		vertexArray->bind();
		RenderCommand::drawIndex(vertexArray);
	}
}