#include "st_pch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Stulu{
	VertexBuffer* VertexBuffer::create(uint32_t size, float* vertices) {
		switch (Renderer::getRendererAPI())
		{
		case RendererAPI::OpenGL:
			return new OpenGLVertexBuffer(size, vertices);
		case RendererAPI::none:
			CORE_ASSERT(false,"No renderAPI specified")
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in VertexBuffer creation")
		return nullptr;
	}

	IndexBuffer* IndexBuffer::create(uint32_t size, uint32_t* indices) {
		switch (Renderer::getRendererAPI())
		{
		case RendererAPI::OpenGL:
			return new OpenGLIndexBuffer(size, indices);
		case RendererAPI::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in IndexBuffer creation")
			return nullptr;
	}

}