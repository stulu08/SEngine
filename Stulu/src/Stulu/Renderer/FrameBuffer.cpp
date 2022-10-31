#include "st_pch.h"
#include "FrameBuffer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Stulu/Renderer/Renderer.h"

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
	Ref<FrameBufferTexture> FrameBufferTexture::create(FrameBufferSpecs& specs) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLFrameBufferTexture>(specs);
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