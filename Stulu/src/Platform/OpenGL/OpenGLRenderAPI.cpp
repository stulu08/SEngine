#include "st_pch.h"
#include "OpenGLRenderAPI.h"
#include <stb_image.h>
#include <glad/glad.h>

namespace Stulu {

	void OpenGLRenderAPI::init() {
		ST_PROFILING_FUNCTION();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void OpenGLRenderAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
		glViewport(x, y, width, height);
	}
	void OpenGLRenderAPI::setDepthFunc(bool lequalLess) {
		ST_PROFILING_FUNCTION();
		glDepthFunc(lequalLess ? GL_LEQUAL : GL_LESS);
	}
	void OpenGLRenderAPI::setDepthMask(bool value) {
		ST_PROFILING_FUNCTION();
		glDepthMask(value);
	}
	void OpenGLRenderAPI::setWireFrame(bool value) {
		ST_PROFILING_FUNCTION();
		if(value)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void OpenGLRenderAPI::setClearColor(const glm::vec4& color) {
		ST_PROFILING_FUNCTION();
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRenderAPI::clear() {
		ST_PROFILING_FUNCTION();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRenderAPI::drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t count) {
		ST_PROFILING_FUNCTION();

		if(count == 0)
			glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
		else
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void OpenGLRenderAPI::drawPrimitiveArray(const uint32_t count) {
		ST_PROFILING_FUNCTION();
		glDrawArrays(GL_TRIANGLES, 0, count);
	}
}