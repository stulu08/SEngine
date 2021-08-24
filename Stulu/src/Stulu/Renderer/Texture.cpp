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
	Ref<Texture2D> Texture2D::create(uint32_t width, uint32_t height)
	{
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(width, height);
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
	Ref<CubeMap> CubeMap::create(const std::vector<std::string>& faces)
	{
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return createRef<OpenGLCubeMap>(faces);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Cube´Map creation");
		return nullptr;
	}
}