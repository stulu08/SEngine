#pragma once
#include "Stulu/Renderer/Buffer.h"

namespace Stulu {
	static inline uint32_t BufferDrawModeToGl(BufferDrawMode mode) {
		switch (mode)
		{
		case Stulu::BufferDrawMode::Static:
			return 0x88E4; //GL_STATIC_DRAW;
		case Stulu::BufferDrawMode::Dynamic:
			return 0x88E8; //GL_DYNAMIC_DRAW;
		case Stulu::BufferDrawMode::Stream:
			return 0x88E0; //GL_STREAM_DRAW;
		}
		return 0x88E4; //GL_STATIC_DRAW;;
	}
	class STULU_API OpenGLVertexBuffer : public VertexBuffer {
	public:
		OpenGLVertexBuffer(uint32_t size, BufferDrawMode mode);
		OpenGLVertexBuffer(uint32_t size, float* vertices, BufferDrawMode mode);
		OpenGLVertexBuffer(uint32_t size, const void* data, BufferDrawMode mode);

		virtual ~OpenGLVertexBuffer();

		virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }
		virtual void setData(const void* data, uint32_t size, uint32_t offset) override;


		virtual void bind() const override;
		virtual void unbind() const override;

		virtual const BufferLayout& getLayout() const override { return m_layout; }
		virtual uint32_t getStride() const override { return getLayout().getStride(); };
		virtual uint32_t getSize() const override { return m_size; }
		virtual uint32_t getCount() const override { return uint32_t((float)m_size / (float)getStride()); }
		virtual BufferDrawMode getBufferDrawMode() const override { return m_mode; }
		virtual void* getNativeRendererObject() const override { return (void*)(&m_rendererID); }
	private:
		uint32_t m_rendererID;
		uint32_t m_size;
		BufferLayout m_layout;
		BufferDrawMode m_mode;
	};


	class STULU_API OpenGLIndexBuffer : public IndexBuffer {
	public:
		OpenGLIndexBuffer(uint32_t count, uint32_t* indices, BufferDrawMode mode);
		virtual ~OpenGLIndexBuffer();

		virtual void setData(const void* data, uint32_t size, uint32_t offset) override;

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual uint32_t getCount() const override { return m_count; }
		virtual uint32_t getSize() const override { return m_count * getStride(); }
		virtual uint32_t getStride() const override { return (uint32_t)sizeof(uint32_t); };
		virtual BufferDrawMode getBufferDrawMode() const override { return m_mode; }
		virtual void* getNativeRendererObject() const override { return (void*)(&m_rendererID); }
	private:
		uint32_t m_rendererID;
		uint32_t m_count;
		BufferDrawMode m_mode;
	};


	class STULU_API OpenGLUniformBuffer : public UniformBuffer {
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();

		virtual void setData(const void* data) override { setData(data, m_size, 0); }
		virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) override;

		virtual void bind(uint32_t binding) const override;

		virtual uint32_t getCount() const override { return uint32_t((float)m_size / (float)getStride());; }
		virtual uint32_t getSize() const override { return m_size; }
		virtual uint32_t getStride() const override { return (uint32_t)sizeof(glm::vec4); };
		virtual BufferDrawMode getBufferDrawMode() const override { return BufferDrawMode::Dynamic; }
		virtual void* getNativeRendererObject() const override { return (void*)(&m_rendererID); }
	private:
		uint32_t m_rendererID = 0;
		uint32_t m_size = 0;
	};


	class STULU_API OpenGLShaderStorageBuffer : public ShaderStorageBuffer {
	public:
		OpenGLShaderStorageBuffer(uint32_t size, uint32_t binding, BufferDrawMode mode);
		virtual ~OpenGLShaderStorageBuffer();

		virtual void setData(const void* data) override { setData(data, m_size, 0); }
		virtual void setData(const void* data, uint32_t size, uint32_t offset) override;

		virtual void bind(uint32_t binding) const override;

		virtual void getData(void* data) const override { getData(data, m_size, 0); }
		virtual void getData(void* data, uint32_t size, uint32_t offset = 0) const override;
		virtual uint32_t getCount() const override { return 0; }
		virtual uint32_t getSize() const override { return m_size; }
		virtual uint32_t getStride() const override { return 1; };
		virtual BufferDrawMode getBufferDrawMode() const override { return m_mode; }
		virtual void* getNativeRendererObject() const override { return (void*)(&m_rendererID); }
	private:
		uint32_t m_rendererID;
		uint32_t m_size;
		BufferDrawMode m_mode;
	};


}