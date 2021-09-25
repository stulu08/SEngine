#include "st_pch.h"
#include "UniformBuffer.h"

#include "Stulu/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Stulu {
	Ref<UniformBuffer> UniformBuffer::create(uint32_t size, uint32_t binding) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLUniformBuffer>(size, binding);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in UniformBuffer creation");
		return nullptr;
	}

}