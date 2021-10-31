#include "st_pch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Stulu {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type)
		{
		case Stulu::ShaderDataType::Float:		return GL_FLOAT;
		case Stulu::ShaderDataType::Float2:		return GL_FLOAT;
		case Stulu::ShaderDataType::Float3:		return GL_FLOAT;
		case Stulu::ShaderDataType::Float4:		return GL_FLOAT;
		case Stulu::ShaderDataType::Int:		return GL_INT;
		case Stulu::ShaderDataType::Int2:		return GL_INT;
		case Stulu::ShaderDataType::Int3:		return GL_INT;
		case Stulu::ShaderDataType::Int4:		return GL_INT;
		case Stulu::ShaderDataType::Mat3:		return GL_FLOAT;
		case Stulu::ShaderDataType::Mat4:		return GL_FLOAT;
		case Stulu::ShaderDataType::Bool:		return GL_INT;
		case Stulu::ShaderDataType::Sampler:	return GL_INT;
		}
		CORE_ASSERT(false, "Uknown ShaderDataType");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		ST_PROFILING_FUNCTION();
		glCreateVertexArrays(1, &m_rendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray() {
		ST_PROFILING_FUNCTION();
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vBuffer) {
		ST_PROFILING_FUNCTION();
		CORE_ASSERT(vBuffer->getLayout().getElements().size(), "Vertexbuffer has no layout");
		glBindVertexArray(m_rendererID);
		vBuffer->bind();

		const auto& layout = vBuffer->getLayout();
		for (const auto& element : layout) {
			glEnableVertexAttribArray(m_vertexbufferIndex);
			glVertexAttribPointer(
				m_vertexbufferIndex, element.getComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(), (const void*)(intptr_t)element.offset
			);
			m_vertexbufferIndex++;
		}
		m_vertexBufffers.push_back(vBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& iBuffer) {
		ST_PROFILING_FUNCTION();
		iBuffer->bind();
		m_indexBuffer = iBuffer;
	}

	void OpenGLVertexArray::bind() const {
		glBindVertexArray(m_rendererID);
	}

	void OpenGLVertexArray::unbind() const {
		ST_PROFILING_FUNCTION();
		glBindVertexArray(0);
	}
}
