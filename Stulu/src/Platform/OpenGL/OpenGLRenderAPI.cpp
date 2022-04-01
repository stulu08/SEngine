#include "st_pch.h"
#include "OpenGLRenderAPI.h"
#include <stb_image.h>
#include <glad/glad.h>

namespace Stulu {
	//https://www.khronos.org/opengl/wiki/Debug_Output
	void debugCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum messageSeverity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		switch (messageSeverity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			CORE_CRITICAL("[OPENGL][Debug] {0}", message)
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			CORE_WARN("[OPENGL][Debug] {0}", message)
			break;
		case GL_DEBUG_SEVERITY_LOW:
			CORE_INFO("[OPENGL][Debug] {0}", message)
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			CORE_TRACE("[OPENGL][Debug] {0}", message)
			break;
		default:
			break;
		}
	}

	void OpenGLRenderAPI::init() {
		ST_PROFILING_FUNCTION();
		glDepthMask(GL_TRUE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		setDefault();
#ifdef ST_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(debugCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#if ST_GRAPHICS_API_VERBOSE_LOGGING == 0
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
#if ST_GRAPHICS_API_INFO_LOGGING == 0
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
#endif
		
#endif

	}

	void OpenGLRenderAPI::setDefault() {
		ST_PROFILING_FUNCTION();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LESS);
	}

	void OpenGLRenderAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
		glViewport(x, y, width, height);
	}
	void OpenGLRenderAPI::setWireFrame(bool value) {
		ST_PROFILING_FUNCTION();
		if(value)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void OpenGLRenderAPI::setCullMode(CullMode v) {
		ST_PROFILING_FUNCTION();
		switch (v)
		{
		case Stulu::CullMode::Back:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case Stulu::CullMode::Front:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		case Stulu::CullMode::BackAndFront:
			glDisable(GL_CULL_FACE);
			break;
		}
	}

	void OpenGLRenderAPI::setStencil(StencilMode v) {
		ST_PROFILING_FUNCTION();
		switch (v)
		{
		case Stulu::StencilMode::DisableWriting:
			glStencilMask(0x00);
			break;
		case Stulu::StencilMode::WriteToBuffer:
			glStencilMask(0xFF);
			glClear(GL_STENCIL_BUFFER_BIT);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			break;
		case Stulu::StencilMode::BeginDrawFromBuffer:
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			setCullMode(CullMode::BackAndFront);
			break;
		case Stulu::StencilMode::EndDrawFromBuffer:
			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glEnable(GL_DEPTH_TEST);
			break;
		}
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
		uint32_t _count = count ? count : vertexArray->getIndexBuffer()->getCount();
		glDrawElements(GL_TRIANGLES, _count, GL_UNSIGNED_INT, nullptr);
	}
	void OpenGLRenderAPI::drawLines(const Ref<VertexArray>& vertexArray, const uint32_t count) {
		ST_PROFILING_FUNCTION();
		uint32_t _count = count ? count : vertexArray->getIndexBuffer()->getCount();
		glDrawElements(GL_LINES, _count, GL_UNSIGNED_INT, nullptr);
	}
}