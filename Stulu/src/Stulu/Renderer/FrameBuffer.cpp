#include "st_pch.h"
#include "FrameBuffer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Stulu {
	Ref<FrameBuffer> FrameBuffer::create(const FrameBufferSpecs& frameBufferdata) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLFramebuffer>(frameBufferdata);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in FrameBuffer creation")
			return nullptr;
	}
	Ref<FrameBufferTexture> FrameBufferTexture::create(uint32_t width, uint32_t height) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLFrameBufferTexture>(width, height);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in FrameBufferTexture creation")
			return nullptr;
	}
}