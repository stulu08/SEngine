#include "st_pch.h"
#include "OpenGLRenderAPI.h"
#include <stb_image.h>
#include <glad/glad.h>
#include "OpenGLStateCache.h"

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
		glDepthMask(GL_TRUE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
		setDefault();


#ifdef ST_GRAPHICS_API_DEBUG_LOGGING
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
		OpenGLStateCache::Reset();

		setCullMode(CullMode::BackAndFront);
		setDepthTesting(true);
		glDepthFunc(GL_LESS);

		StencilAlways(0xFF, 0);
		SetStencilValue(0x00);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glLineWidth(1.0f);

		

	}

	void OpenGLRenderAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}
	void OpenGLRenderAPI::setWireFrame(bool value) {
		glPolygonMode(GL_FRONT_AND_BACK, value ? GL_LINE : GL_FILL);
	}

	void OpenGLRenderAPI::setCullMode(CullMode v) {
		switch (v)
		{
		case Stulu::CullMode::Back:
			OpenGLStateCache::SetCullFace(GL_BACK);
			break;
		case Stulu::CullMode::Front:
			OpenGLStateCache::SetCullFace(GL_FRONT);
			break;
		case Stulu::CullMode::BackAndFront:
			OpenGLStateCache::SetCullFace(GL_FRONT_AND_BACK);
			break;
		}
	}
	//will make this more usefull but this is enough for now
	void OpenGLRenderAPI::StencilNotEqual(uint8_t val, uint8_t ref) {
		glStencilFunc(GL_NOTEQUAL, ref, val);
	}

	void OpenGLRenderAPI::SetStencilValue(uint8_t value) {
		OpenGLStateCache::StencilMask(value);
	}

	void OpenGLRenderAPI::StencilAlways(uint8_t value, uint8_t ref) {
		glStencilFunc(GL_ALWAYS, ref, value);
	}

	void OpenGLRenderAPI::setDepthTesting(bool v) {
		if (v) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
	}

	void OpenGLRenderAPI::setClearColor(const glm::vec4& color) {
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRenderAPI::clear() {
		OpenGLStateCache::StencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRenderAPI::drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t indicesCount, const uint32_t instanceCount) {
		vertexArray->bind();
		const uint32_t count = indicesCount ? indicesCount : vertexArray->getIndexBuffer()->getCount();

		if (instanceCount > 1) {
			glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instanceCount);
		}
		else {
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		}
	}

	void OpenGLRenderAPI::drawIndexedSubMesh(const Ref<VertexArray>& vertexArray, const uint32_t indicesCount, const uint32_t indexOffset, const uint32_t vertexOffset, const uint32_t instanceCount) {
		vertexArray->bind();

		if (instanceCount > 1) {
			glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(uint32_t)), instanceCount);
		}
		else {
			glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(uint32_t)));
		}
	}

	void OpenGLRenderAPI::drawLines(const Ref<VertexArray>& vertexArray, const uint32_t count) {
		vertexArray->bind();
		glDrawArrays(GL_LINES, 0, count);
	}

	const glm::ivec3 OpenGLRenderAPI::getMaxComputeWorkGroupCount() const {
		glm::ivec3 count = { 0,0,0 };
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &count.x);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &count.y);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &count.z);
		return count;
	}
	const glm::ivec3 OpenGLRenderAPI::getMaxComputeWorkGroupSizes() const {
		glm::ivec3 size = { 0,0,0 };
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &size.x);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &size.y);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &size.z);
		return size;
	}
	const uint32_t OpenGLRenderAPI::getMaxComputeWorkGroupInvocationCount() const {
		int32_t invc = 0;
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &invc);
		return (uint32_t)invc;
	}
}