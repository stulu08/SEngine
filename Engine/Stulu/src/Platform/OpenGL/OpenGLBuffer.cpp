#include "st_pch.h"
#include "OpenGLBuffer.h"
#include <glad/glad.h>

namespace Stulu {

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, BufferDrawMode mode) 
		: m_size(size), m_mode(mode) {
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, BufferDrawModeToGl(m_mode));
	}
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, float* vertices, BufferDrawMode mode) 
		: m_size(size), m_mode(mode) {
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, BufferDrawModeToGl(m_mode));
	}
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, const void* data, BufferDrawMode mode) 
		: m_size(size), m_mode(mode) {
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, BufferDrawModeToGl(m_mode));
	}
	OpenGLVertexBuffer::~OpenGLVertexBuffer() {
		glDeleteBuffers(1, &m_rendererID);
	}
	void OpenGLVertexBuffer::setData(const void* data, uint32_t size, uint32_t offset) {
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		m_size = size;
	}
	void OpenGLVertexBuffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	}
	void OpenGLVertexBuffer::unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//------------//

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count, uint32_t* indices, BufferDrawMode mode)
		: m_count(count), m_mode(mode) {
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, BufferDrawModeToGl(m_mode));
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer() {
		glDeleteBuffers(1, &m_rendererID);
	}
	void OpenGLIndexBuffer::bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	}
	void OpenGLIndexBuffer::unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void OpenGLIndexBuffer::setData(const void* data, uint32_t size, uint32_t offset) {
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		m_count = uint32_t((float)size / (float)sizeof(uint32_t));
	}

	//------------//

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
		: m_size(size) {
		glCreateBuffers(1, &m_rendererID);
		glNamedBufferData(m_rendererID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_rendererID);
	}
	OpenGLUniformBuffer::~OpenGLUniformBuffer() {
		glDeleteBuffers(1, &m_rendererID);
	}
	void OpenGLUniformBuffer::setData(const void* data, uint32_t size, uint32_t offset) {
		glNamedBufferSubData(m_rendererID, offset, size, data);
	}
	void OpenGLUniformBuffer::bind(uint32_t binding) const {
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_rendererID);
	}

	//------------//

	OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(uint32_t size, uint32_t binding, BufferDrawMode mode)
		: m_size(size), m_mode(mode) {
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_rendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, BufferDrawModeToGl(m_mode));
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_rendererID);
	}
	OpenGLShaderStorageBuffer::~OpenGLShaderStorageBuffer() {
		glDeleteBuffers(1, &m_rendererID);
	}
	void OpenGLShaderStorageBuffer::setData(const void* data, uint32_t size, uint32_t offset) {
		glNamedBufferSubData(m_rendererID, offset, size, data);
	}
	void OpenGLShaderStorageBuffer::bind(uint32_t binding) const {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_rendererID);
	}

	void OpenGLShaderStorageBuffer::getData(void* data, uint32_t size, uint32_t offset) const {
		glGetNamedBufferSubData(m_rendererID, offset, size, data);
	}
}