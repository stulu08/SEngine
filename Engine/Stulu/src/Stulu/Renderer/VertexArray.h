#pragma once
#include "Stulu/Renderer/Buffer.h"

namespace Stulu {
	class STULU_API VertexArray {
	public:
		virtual ~VertexArray() = default;

		virtual void addVertexBuffer(const Ref<VertexBuffer>& vBuffer) = 0;
		virtual void clearVertexBuffers() = 0;
		virtual void setIndexBuffer(const Ref<IndexBuffer>& iBuffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& getIndexBuffer() const = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;


		static Ref<VertexArray> create();
	};
}