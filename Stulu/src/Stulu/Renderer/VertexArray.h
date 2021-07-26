#pragma once
#include "Stulu/Renderer/Buffer.h"

namespace Stulu {
	class VertexArray {
	public:
		virtual ~VertexArray() {};

		virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vBuffer) = 0;
		virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& iBuffer) = 0;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const = 0;
		virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;


		static VertexArray* create();
	};
}