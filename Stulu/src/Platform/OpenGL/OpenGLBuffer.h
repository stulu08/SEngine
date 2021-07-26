#pragma once
#include "Stulu/Renderer/Buffer.h"

namespace Stulu {
	class OpenGLVertexBuffer : public VertexBuffer {
	public:
		OpenGLVertexBuffer(uint32_t size, float* vertices);

		virtual ~OpenGLVertexBuffer();


		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }
		virtual const BufferLayout& getLayout() const override { return m_layout; }
	private:
		uint32_t m_rendererID;
		BufferLayout m_layout;
	};


	class OpenGLIndexBuffer : public IndexBuffer {
	public:
		OpenGLIndexBuffer(uint32_t size, uint32_t* indices);
		virtual ~OpenGLIndexBuffer();


		virtual void bind() const override;
		virtual void unbind() const override;

		virtual uint32_t getCount() const { return m_count; }

	private:
		uint32_t m_rendererID;
		uint32_t m_count;

	};
}