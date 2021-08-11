#include "st_pch.h"
#include "Buffer.h"

#include "Stulu/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Stulu{
	Ref<VertexBuffer> VertexBuffer::create(uint32_t size, float* vertices) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(size, vertices);
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
	Ref<VertexBuffer> VertexBuffer::create(uint32_t size, const void* data) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(size, data);
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

	Ref<IndexBuffer> IndexBuffer::create(uint32_t size, uint32_t* indices) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLIndexBuffer>(size, indices);
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