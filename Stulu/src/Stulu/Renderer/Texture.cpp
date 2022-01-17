#include "st_pch.h"
#include "Texture.h"

#include "Stulu/Scene/YAML.h"
#include "Stulu/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Stulu/Scene/AssetsManager.h"
namespace Stulu {
	Ref<Texture2D> Texture2D::create(const std::string& path, const TextureSettings& settings)
	{
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(path,settings);
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
	Ref<CubeMap> CubeMap::create(uint32_t width, uint32_t height)
	{
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return createRef<OpenGLCubeMap>(width, height);
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
	Ref<CubeMap> CubeMap::create(const std::string& cubeMapYamlPath) {
		YAML::Node data = YAML::LoadFile(cubeMapYamlPath);
		std::string right = AssetsManager::get(data["right"].as<uint64_t>()).path;
		std::string left = AssetsManager::get(data["left"].as<uint64_t>()).path;
		std::string top = AssetsManager::get(data["top"].as<uint64_t>()).path;
		std::string bottom = AssetsManager::get(data["bottom"].as<uint64_t>()).path;
		std::string front = AssetsManager::get(data["front"].as<uint64_t>()).path;
		std::string back = AssetsManager::get(data["back"].as<uint64_t>()).path;
		return create({ right,left,top,bottom,front,back });
	}
}