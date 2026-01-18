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
	Ref<Texture2D> Texture2D::create(uint32_t width, uint32_t height, const TextureSettings& settings, MSAASamples samples)
	{
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(width, height, settings, samples);
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


	Ref<SkyBox> SkyBox::CreateFromFacesList(const std::vector<std::string>& faces, uint32_t resolution) {
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
	Ref<SkyBox> SkyBox::CreateFromYaml(const std::string& cubeMapYamlPath, uint32_t resolution) {
		YAML::Node data = YAML::LoadFile(cubeMapYamlPath);
		std::string right = AssetsManager::GlobalInstance().GetRaw(data["right"].as<UUID>())->GetPath();
		std::string left = AssetsManager::GlobalInstance().GetRaw(data["left"].as<UUID>())->GetPath();
		std::string top = AssetsManager::GlobalInstance().GetRaw(data["top"].as<UUID>())->GetPath();
		std::string bottom = AssetsManager::GlobalInstance().GetRaw(data["bottom"].as<UUID>())->GetPath();
		std::string front = AssetsManager::GlobalInstance().GetRaw(data["front"].as<UUID>())->GetPath();
		std::string back = AssetsManager::GlobalInstance().GetRaw(data["back"].as<UUID>())->GetPath();
		return SkyBox::CreateFromFacesList({ right,left,top,bottom,front,back }, resolution);
	}
	Ref<SkyBox> SkyBox::Create(const std::string& path, uint32_t resolution) {

		if (path.substr(path.find_last_of('.'), path.npos) == ".skybox")
			return SkyBox::CreateFromYaml(path, resolution);
		return SkyBox::CreateFromEquirectangularMap(path, resolution);
	}
	Ref<SkyBox> SkyBox::CreateFromEquirectangularMap(const std::string& path, uint32_t resolution) {

		if (path.substr(path.find_last_of('.'), path.npos) == ".skybox")
			return SkyBox::CreateFromYaml(path, resolution);

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
		return nullptr;
	}

	Ref<CubeMap> Stulu::CubeMap::create(uint32_t resolution, const std::vector<std::string>& faces, TextureSettings settings)
	{
		switch (Renderer::getRendererAPI())
		{
		case Renderer::API::OpenGL:
			return createRef<OpenGLCubeMap>(resolution, faces, settings);
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

	Ref<Texture2D> SkyBox::genrateBRDFLUT(uint32_t resolution) {
		auto& assetsManaer = AssetsManager::GlobalInstance();
		UUID textureId = UUID("BRDFLUT_TEXTURE_" + std::to_string(resolution));

		if (!assetsManaer.Contains(textureId)) {
			auto specs = FrameBufferSpecs();
			specs.width = resolution;
			specs.height = resolution;

			auto colorTexture = TextureSettings(TextureFormat::RG16F);
			colorTexture.filtering = TextureFiltering::Linear;
			colorTexture.wrap = TextureWrap::ClampToEdge;

			auto depthTexture = TextureSettings(TextureFormat::Depth24);
			depthTexture.filtering = TextureFiltering::Nearest;
			depthTexture.wrap = TextureWrap::ClampToEdge;

			auto framebuffer = FrameBuffer::create(specs, colorTexture, depthTexture);

			RenderCommand::SetBlending(false);
			RenderCommand::clear();
			Renderer::ScreenQuad(framebuffer, Resources::GetBRDFLutShader());
			RenderCommand::SetBlending(true);

			SharedTexture2DAssetData* asset = new SharedTexture2DAssetData(textureId, framebuffer->getColorAttachment());
			assetsManaer.AddAsset(asset, textureId, true);
		}

		return assetsManaer.GetAsset<Texture2DAsset>(textureId).GetAsset()->GetTextureHandle();
	}
}