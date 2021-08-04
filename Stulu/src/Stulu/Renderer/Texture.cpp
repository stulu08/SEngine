#include "st_pch.h"
#include "Texture.h"

#include "Stulu/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Stulu {
	Ref<Texture2D> Texture2D::create(const std::string& path)
	{
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(path);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Texture2D creation");
		return nullptr;
	}
}