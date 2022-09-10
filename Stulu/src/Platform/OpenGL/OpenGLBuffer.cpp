#include "st_pch.h"
#include "OpenGLBuffer.h"
#include <glad/glad.h>

namespace Stulu {

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, BufferDrawMode mode) 
		: m_size(size), m_mode(mode) {
		ST_PROFILING_FUNCTION();
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, BufferDrawModeToGl(m_mode));

	}
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, float* vertices, BufferDrawMode mode) 
		: m_size(size), m_mode(mode) {
		ST_PROFILING_FUNCTION();
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, BufferDrawModeToGl(m_mode));
	}
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, const void* data, BufferDrawMode mode) 
		: m_size(size), m_mode(mode) {
		ST_PROFILING_FUNCTION();
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, BufferDrawModeToGl(m_mode));
	}
	OpenGLVertexBuffer::~OpenGLVertexBuffer() {
		ST_PROFILING_FUNCTION();
		glDeleteBuffers(1, &m_rendererID);
	}
	void OpenGLVertexBuffer::setData(const void* data, uint32_t size) {
		ST_PROFILING_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		m_size = size;
	}
	void OpenGLVertexBuffer::bind() const {
		ST_PROFILING_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	}
	void OpenGLVertexBuffer::unbind() const {
		ST_PROFILING_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//------------//

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count, uint32_t* indices, BufferDrawMode mode)
		: m_count(count), m_mode(mode) {
		ST_PROFILING_FUNCTION();
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, BufferDrawModeToGl(m_mode));

	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer() {
		ST_PROFILING_FUNCTION();
		glDeleteBuffers(1, &m_rendererID);
	}
	void OpenGLIndexBuffer::bind() const {
		ST_PROFILING_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	}
	void OpenGLIndexBuffer::unbind() const {
		ST_PROFILING_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void OpenGLIndexBuffer::setData(const void* data, uint32_t size) {
		ST_PROFILING_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		m_count = uint32_t((float)size / (float)sizeof(uint32_t));
	}
}