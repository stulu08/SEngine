#include "st_pch.h"
#include "Renderer.h"

namespace Stulu {
	void Renderer::beginScene() {

	}
	void Renderer::endScene() {

	}
	void Renderer::submit(const std::shared_ptr<VertexArray>& vertexArray) {
		vertexArray->bind();
		RenderCommand::drawIndex(vertexArray);
	}
}