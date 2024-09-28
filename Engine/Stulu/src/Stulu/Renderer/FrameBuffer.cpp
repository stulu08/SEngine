#include "st_pch.h"
#include "FrameBuffer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	Ref<FrameBuffer> FrameBuffer::create(const FrameBufferSpecs& frameBufferdata, const TextureSettings& defaultCB, const TextureSettings& defaultDB) {
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return std::make_shared<OpenGLFramebuffer>(frameBufferdata, defaultCB, defaultDB);
		case Renderer::API::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in FrameBuffer creation")
			return nullptr;
	}

	Ref<FrameBuffer> FrameBuffer::createEmpty(const FrameBufferSpecs& frameBufferdata) {
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return std::make_shared<OpenGLFramebuffer>(frameBufferdata);
		case Renderer::API::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in FrameBuffer creation")
			return nullptr;
	}
}