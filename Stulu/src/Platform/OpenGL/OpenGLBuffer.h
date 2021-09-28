#pragma once
#include "Stulu/Renderer/Buffer.h"

namespace Stulu {
	class OpenGLVertexBuffer : public VertexBuffer {
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(uint32_t size, float* vertices);
		OpenGLVertexBuffer(uint32_t size, const void* data);

		virtual ~OpenGLVertexBuffer();

		virtual void setLayout(const BufferLayout& layout) override { m_defaultLayout = layout; }
		virtual const BufferLayout& getLayout() const override { return m_defaultLayout; }
		virtual void setData(const void* data, uint32_t size) override;


		virtual void bind() const override;
		virtual void unbind() const override;

	private:
		uint32_t m_rendererID;
		BufferLayout m_defaultLayout;
	};


	class OpenGLIndexBuffer : public IndexBuffer {
	public:
		OpenGLIndexBuffer(uint32_t count, uint32_t* indices);
		virtual ~OpenGLIndexBuffer();


		virtual void bind() const override;
		virtual void unbind() const override;

		virtual uint32_t getCount() const { return m_count; }

	private:
		uint32_t m_rendererID;
		uint32_t m_count;

	};
}