#pragma once
#include "Stulu/Renderer/VertexArray.h"

namespace Stulu {
	class STULU_API OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void addVertexBuffer(const Ref<VertexBuffer>& vBuffer) override;
		virtual void clearVertexBuffers() override;
		virtual void setIndexBuffer(const Ref<IndexBuffer>& iBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const override{ return m_vertexBufffers; }
		virtual const Ref<IndexBuffer>& getIndexBuffer() const override{ return m_indexBuffer; }

		virtual void bind() const override;
		virtual void unbind() const override;
	private:
		uint32_t m_rendererID;
		uint32_t m_vertexbufferIndex = 0;
		std::vector<Ref<VertexBuffer>> m_vertexBufffers;
		Ref<IndexBuffer> m_indexBuffer;
	};
}
