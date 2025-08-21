#include "st_pch.h"
#include "OpenGLVertexArray.h"
#include "OpenGLStateCache.h"

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
		glCreateVertexArrays(1, &m_rendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vBuffer) {
		CORE_ASSERT(vBuffer->getLayout().getElements().size(), "Vertexbuffer has no layout");
		OpenGLStateCache::BindVertexArray(m_rendererID);
		vBuffer->bind();
		const auto& layout = vBuffer->getLayout();
		for (const auto& element : layout) {
			glEnableVertexAttribArray(m_vertexbufferIndex);
			GLenum dataType = ShaderDataTypeToOpenGLBaseType(element.type);
			if (dataType == GL_INT) {
				glVertexAttribIPointer(
					m_vertexbufferIndex, element.getComponentCount(),
					dataType,
					layout.getStride(), (const void*)(intptr_t)element.offset
				);
			}
			else {
				glVertexAttribPointer(
					m_vertexbufferIndex, element.getComponentCount(),
					dataType,
					element.normalized ? GL_TRUE : GL_FALSE,
					layout.getStride(), (const void*)(intptr_t)element.offset
				);
			}

			
			m_vertexbufferIndex++;
		}
		m_vertexBufffers.push_back(vBuffer);
	}

	void OpenGLVertexArray::clearVertexBuffers() {
		OpenGLStateCache::BindVertexArray(m_rendererID);
		for (auto& vb : m_vertexBufffers) {
			vb->unbind();
		}
		m_vertexbufferIndex = 0;
		m_vertexBufffers.clear();
	}

	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& iBuffer) {
		OpenGLStateCache::BindVertexArray(m_rendererID);
		iBuffer->bind();
		m_indexBuffer = iBuffer;
	}

	void OpenGLVertexArray::bind() const {
		OpenGLStateCache::BindVertexArray(m_rendererID);
	}

	void OpenGLVertexArray::unbind() const {
		OpenGLStateCache::BindVertexArray(0);
	}
}
