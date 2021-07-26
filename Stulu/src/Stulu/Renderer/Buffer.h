#pragma once
namespace Stulu{
	class VertexBuffer {
	public:
		virtual ~VertexBuffer() {};


		virtual void bind() const = 0;
		virtual void unbind() const = 0;


		static VertexBuffer* create(uint32_t size, float* vertices);
	};
	class IndexBuffer {
	public:
		virtual ~IndexBuffer() {};

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual uint32_t getCount() const = 0;

		static IndexBuffer* create(uint32_t size, uint32_t* vertices);
	};
}