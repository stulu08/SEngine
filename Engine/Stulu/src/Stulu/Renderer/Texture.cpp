#include "st_pch.h"
#include "Texture.h"

#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL/OpenGLTextureCubeMap.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Serialization/YAML.h"

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
	Ref<Texture2D> Texture2D::create(Ref<Texture2D> copy) {
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(copy);
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

	Ref<SkyBox> SkyBox::create(const std::vector<std::string>& faces, uint32_t resolution) {
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
		std::string right = AssetsManager::GlobalInstance().GetRaw(data["right"].as<UUID>())->GetPath();
		std::string left = AssetsManager::GlobalInstance().GetRaw(data["left"].as<UUID>())->GetPath();
		std::string top = AssetsManager::GlobalInstance().GetRaw(data["top"].as<UUID>())->GetPath();
		std::string bottom = AssetsManager::GlobalInstance().GetRaw(data["bottom"].as<UUID>())->GetPath();
		std::string front = AssetsManager::GlobalInstance().GetRaw(data["front"].as<UUID>())->GetPath();
		std::string back = AssetsManager::GlobalInstance().GetRaw(data["back"].as<UUID>())->GetPath();
		return create({ right,left,top,bottom,front,back }, resolution);
	}
	Ref<SkyBox> SkyBox::create(const std::string& path, uint32_t resolution) {

		if (path.substr(path.find_last_of('.'), path.npos) == ".skybox")
			return SkyBox::createYAML(path, resolution);

		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return createRef<OpenGLSkyBox>(path, resolution);
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
	void SkyBox::UpdateSkybox(const std::string& path, uint32_t resolution) {
		if (path.substr(path.find_last_of('.'), path.npos) == ".hdr")
			return UpdateSkyboxYAML(path, resolution);
		else
			return update(path, resolution);
	}

	void SkyBox::UpdateSkyboxYAML(const std::string& cubeMapYamlPath, uint32_t resolution) {
		YAML::Node data = YAML::LoadFile(cubeMapYamlPath);
		std::string right = AssetsManager::GlobalInstance().GetRaw(data["right"].as<UUID>())->GetPath();
		std::string left = AssetsManager::GlobalInstance().GetRaw(data["left"].as<UUID>())->GetPath();
		std::string top = AssetsManager::GlobalInstance().GetRaw(data["top"].as<UUID>())->GetPath();
		std::string bottom = AssetsManager::GlobalInstance().GetRaw(data["bottom"].as<UUID>())->GetPath();
		std::string front = AssetsManager::GlobalInstance().GetRaw(data["front"].as<UUID>())->GetPath();
		std::string back = AssetsManager::GlobalInstance().GetRaw(data["back"].as<UUID>())->GetPath();
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
				break;
			default:
				CORE_ASSERT(false, "RenderAPI not suported");
				break;
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