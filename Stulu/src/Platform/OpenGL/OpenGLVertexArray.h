#pragma once
#include "Stulu/Renderer/VertexArray.h"

namespace Stulu {
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vBuffer) override;
		virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& iBuffer) override;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const override{ return m_vertexBufffers; }
		virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const override{ return m_indexBuffer; }


		virtual void bind() const override;
		virtual void unbind() const override;
	private:
		uint32_t m_rendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_vertexBufffers;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
	};
}
