#include "st_pch.h"
#include "Texture.h"

#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL/OpenGLTextureCubeMap.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Serialization/YAML.h"
#include "Stulu/Math/CaptureProjection.h"

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

	Ref<CubeMap> CubeMap::Create(uint32_t resolution, TextureSettings settings)
	{
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

	Ref<CubeMap> CubeMap::CreateFromFacesList(uint32_t resolution, const std::vector<std::string>& faces, TextureSettings settings)
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
	Ref<CubeMap> CubeMap::CreateFromYaml(uint32_t resolution, const std::string& cubeMapYamlPath, TextureSettings settings)
	{
		YAML::Node data = YAML::LoadFile(cubeMapYamlPath);
		std::string right = AssetsManager::GlobalInstance().GetRaw(data["right"].as<UUID>())->GetPath();
		std::string left = AssetsManager::GlobalInstance().GetRaw(data["left"].as<UUID>())->GetPath();
		std::string top = AssetsManager::GlobalInstance().GetRaw(data["top"].as<UUID>())->GetPath();
		std::string bottom = AssetsManager::GlobalInstance().GetRaw(data["bottom"].as<UUID>())->GetPath();
		std::string front = AssetsManager::GlobalInstance().GetRaw(data["front"].as<UUID>())->GetPath();
		std::string back = AssetsManager::GlobalInstance().GetRaw(data["back"].as<UUID>())->GetPath();
		return CubeMap::CreateFromFacesList(resolution, { right,left,top,bottom,front,back }, settings);
	}

	Ref<CubeMap> CubeMap::CreateFromEquirectangularMap(uint32_t resolution, const std::string& hdrTexturePath, TextureSettings settings)
	{
		auto depthSettings = TextureSettings(TextureFormat::Depth24);
		depthSettings.wrap = TextureWrap::ClampToEdge;
		depthSettings.filtering = TextureFiltering::Nearest;

		auto captureFramebufferSpecs = FrameBufferSpecs();
		captureFramebufferSpecs.height = resolution;
		captureFramebufferSpecs.width = resolution;
		auto captureFramebuffer = FrameBuffer::create(captureFramebufferSpecs, TextureFormat::None, depthSettings);

		// load the HDR environment map
		auto hdrTextureSettings = TextureSettings(settings.format);
		hdrTextureSettings.wrap = TextureWrap::ClampToEdge;
		hdrTextureSettings.filtering = TextureFiltering::Linear;
		auto hdrTexture = Texture2D::create(hdrTexturePath, hdrTextureSettings);

		if(settings.format == TextureFormat::Auto)
			settings.format = hdrTexture->getSettings().format;

		// setup cubemap to render to and attach to framebuffer
		auto cubeMap = CubeMap::Create(resolution, settings);

		// convert HDR equirectangular environment map to cubemap equivalent
		auto equirectangularToCubemapShader = Renderer::getShaderSystem()->GetShader("Renderer/CubeMap/EquirectangularToCubemap");

		hdrTexture->bind(0);
		equirectangularToCubemapShader->bind();
		equirectangularToCubemapShader->setMat("projection", CaptureProjection::Projection);
		for (uint32_t i = 0; i < 6; i++)
		{
			captureFramebuffer->attactCubeMapRefrenceTexture(cubeMap, i);
			equirectangularToCubemapShader->setMat("view", CaptureProjection::Views[i]);

			captureFramebuffer->bind();
			RenderCommand::clear();
			Renderer::RenderSkyBoxCube();
			captureFramebuffer->unbind();
		}

		captureFramebuffer.reset();
		hdrTexture.reset();

		return cubeMap;
	}
}