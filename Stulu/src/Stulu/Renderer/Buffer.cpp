#include "st_pch.h"
#include "Buffer.h"

#include "Stulu/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Stulu{
	Ref<VertexBuffer> VertexBuffer::create(uint32_t size, BufferDrawMode mode) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(size, mode);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in VertexBuffer creation");
		return nullptr;
	}
	Ref<VertexBuffer> VertexBuffer::create(uint32_t size, float* vertices, BufferDrawMode mode) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(size, vertices, mode);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in VertexBuffer creation");
		return nullptr;
	}
	Ref<VertexBuffer> VertexBuffer::create(uint32_t size, const void* data, BufferDrawMode mode) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(size, data, mode);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in VertexBuffer creation");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::create(uint32_t count, uint32_t* indices, BufferDrawMode mode) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLIndexBuffer>(count, indices, mode);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in IndexBuffer creation");
			return nullptr;
	}

}