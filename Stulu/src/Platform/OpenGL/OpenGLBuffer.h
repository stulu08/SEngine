#pragma once
#include "Stulu/Renderer/Buffer.h"

namespace Stulu {
	class OpenGLVertexBuffer : public VertexBuffer {
	public:
		OpenGLVertexBuffer(uint32_t size, float* vertices);

		virtual ~OpenGLVertexBuffer();


		virtual void bind() const override;
		virtual void unbind() const override;

	private:
		uint32_t m_rendererID;

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