#include "st_pch.h"
#include "Texture.h"

#include "Stulu/Scene/YAML.h"
#include "Stulu/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL/OpenGLTextureCubeMap.h"
#include "Stulu/Scene/AssetsManager.h"
namespace Stulu {
	Ref<Texture2D> Texture2D::create(const std::string& path, const TextureSettings& settings)
	{
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(path,settings);
		case Renderer::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Texture2D creation");
		return nullptr;
	}
	Ref<Texture2D> Texture2D::create(uint32_t width, uint32_t height, const TextureSettings& settings)
	{
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(width, height, settings);
		case Renderer::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Texture2D creation");
		return nullptr;
	}
	Ref<SkyBox> SkyBox::create(const std::string& path) {
		uint32_t resolution = 512;
		if (AssetsManager::hasProperity(path, "resolution")) {
			resolution = AssetsManager::getProperity<uint32_t>(path, "resolution");
		}
		else {
			AssetsManager::setProperity<uint32_t>(path, { "resolution" ,resolution });
		}

		if (path.substr(path.find_last_of('.'), path.npos) == ".skybox")
			return SkyBox::createYAML(path, resolution);
		else
			return SkyBox::create(path, resolution);
	}
	Ref<SkyBox> SkyBox::create(uint32_t resolution, void* data)
	{
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return createRef<OpenGLSkyBox>(resolution, data);
		case Renderer::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in CubeMap creation");
		return nullptr;
	}
	Ref<SkyBox> SkyBox::create(const std::vector<std::string>& faces, uint32_t resolution)
	{
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return createRef<OpenGLSkyBox>(faces, resolution);
		case Renderer::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in CubeMap creation");
		return nullptr;
	}
	Ref<SkyBox> SkyBox::createYAML(const std::string& cubeMapYamlPath, uint32_t resolution) {
		YAML::Node data = YAML::LoadFile(cubeMapYamlPath);
		std::string right = AssetsManager::get(data["right"].as<uint64_t>()).path;
		std::string left = AssetsManager::get(data["left"].as<uint64_t>()).path;
		std::string top = AssetsManager::get(data["top"].as<uint64_t>()).path;
		std::string bottom = AssetsManager::get(data["bottom"].as<uint64_t>()).path;
		std::string front = AssetsManager::get(data["front"].as<uint64_t>()).path;
		std::string back = AssetsManager::get(data["back"].as<uint64_t>()).path;
		return create({ right,left,top,bottom,front,back }, resolution);
	}
	Ref<SkyBox> SkyBox::create(const std::string& hdrTexturePath, uint32_t resolution)
	{
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return createRef<OpenGLSkyBox>(hdrTexturePath, resolution);
		case Renderer::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in CubeMap creation");
		return nullptr;
	}
	void SkyBox::update(const std::string& path) {
		uint32_t resolution = 2048;
		if (AssetsManager::hasProperity(path, "resolution")) {
			resolution = AssetsManager::getProperity<uint32_t>(path, "resolution");
		}
		else {
			AssetsManager::setProperity<uint32_t>(path, { "resolution" ,resolution });
		}

		if (path.substr(path.find_last_of('.'), path.npos) == ".hdr")
			return update(path, resolution);
		else
			return updateYAML(path, resolution);
	}

	void SkyBox::updateYAML(const std::string& cubeMapYamlPath, uint32_t resolution) {
		YAML::Node data = YAML::LoadFile(cubeMapYamlPath);
		std::string right = AssetsManager::get(data["right"].as<uint64_t>()).path;
		std::string left = AssetsManager::get(data["left"].as<uint64_t>()).path;
		std::string top = AssetsManager::get(data["top"].as<uint64_t>()).path;
		std::string bottom = AssetsManager::get(data["bottom"].as<uint64_t>()).path;
		std::string front = AssetsManager::get(data["front"].as<uint64_t>()).path;
		std::string back = AssetsManager::get(data["back"].as<uint64_t>()).path;
		return update({ right,left,top,bottom,front,back }, resolution);
	}

	Ref<CubeMap> CubeMap::create(uint32_t resolution, TextureSettings settings) {
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return createRef<OpenGLCubeMap>(resolution, settings);
		case Renderer::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in CubeMap creation");
		return nullptr;;
	}


	Ref<Texture>& SkyBox::genrateBRDFLUT(uint32_t resolution) {
		//resolution texture
		static std::unordered_map<uint32_t, Ref<Texture>> textures;

		if (textures.find(resolution) == textures.end() || textures.at(resolution) == nullptr) {
			Ref<Texture> texture = nullptr;
			switch (Renderer::getRendererAPI())
			{
			case Renderer::API::OpenGL:
				texture = OpenGLSkyBox::genrateBRDFLUT(resolution);
				break;
			case Renderer::API::none:
				CORE_ASSERT(false, "No renderAPI specified");
			default:
				CORE_ASSERT(false, "RenderAPI not suported");
			}
			textures[resolution] = texture;
		}

		return textures.at(resolution);
	}


	Ref<ReflectionMap> ReflectionMap::create(uint32_t resolution, TextureSettings settings) {
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return createRef<OpenGLReflectionMap>(resolution, settings);
		case Renderer::API::none:
			CORE_ASSERT(false, "No renderAPI specified");
			return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported");
			return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in CubeMap creation");
		return nullptr;;
	}

}