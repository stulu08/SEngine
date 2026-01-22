#include "st_pch.h"
#include "SkyBox.h"
#include "Renderer.h"
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Math/CaptureProjection.h"

namespace Stulu {
	Ref<SkyBox> SkyBox::Create(uint32_t resolution, const std::string& path, const TextureSettings settings)
	{
		Ref<CubeMap> cubeMap = nullptr;
		if (path.substr(path.find_last_of('.'), path.npos) == ".skybox")
			cubeMap = CubeMap::CreateFromYaml(resolution, path, settings);
		else
			cubeMap = CubeMap::CreateFromEquirectangularMap(resolution, path, settings);

		return SkyBox::CreateFromCubeMap(cubeMap);
	}
	Ref<SkyBox> SkyBox::CreateFromCubeMap(Ref<CubeMap> cubeMap)
	{
		return createRef<SkyBox>(cubeMap);
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

	static Ref<FrameBuffer> GetCaptureBuffer(uint32_t resolution);


	SkyBox::SkyBox(const Ref<CubeMap> cubeMap) 
		: m_resolution(cubeMap->getWidth()), m_environmentMap(cubeMap), m_settings(cubeMap->getSettings().format) {
		m_settings.wrap = TextureWrap::ClampToEdge;
		m_settings.tiling = glm::vec2(1.0f);
		m_settings.filtering = TextureFiltering::Trilinear;

		// let OpenGL generate mipmaps from first mip face (combatting visible dots artifact), Trilinear filtering required
		m_environmentMap->GenerateMips();
		generateMaps();
	}

	void SkyBox::generateMaps() {
		auto captureFramebuffer = GetCaptureBuffer(m_resolution / 16);

		// solve diffuse integral by convolution to create an irradiance map.
		{
			auto irradianceSettings = TextureSettings(m_settings.format);
			irradianceSettings.wrap = m_settings.wrap;
			irradianceSettings.filtering = TextureFiltering::Linear;

			auto irradianceMap = CubeMap::Create(m_resolution / 16, irradianceSettings);

			auto irradianceShader = getIrradianceShader();
			irradianceShader->bind();
			irradianceShader->setMat("projection", CaptureProjection::Projection);
			m_environmentMap->bind(0);

			for (uint32_t i = 0; i < 6; i++)
			{
				irradianceShader->setMat("view", CaptureProjection::Views[i]);
				captureFramebuffer->attactCubeMapRefrenceTexture(irradianceMap, i);

				captureFramebuffer->bind();
				RenderCommand::clear();
				Renderer::RenderSkyBoxCube();
				captureFramebuffer->unbind();
			}

			m_irradianceMap = irradianceMap;
		}

		// run a quasi monte-carlo simulation on the environment lighting to create a prefilter map.
		{
			const auto MAX_MIPS = ST_MAX_REFLECTION_LOD + 1;

			auto prefilterSettings = TextureSettings(m_settings.format);
			prefilterSettings.wrap = m_settings.wrap;
			prefilterSettings.filtering = TextureFiltering::Trilinear;
			prefilterSettings.levels = MAX_MIPS;

			auto prefilterMap = CubeMap::Create(m_resolution / 4, prefilterSettings);

			auto prefilterShader = getPrefilterShader();
			prefilterShader->bind();
			prefilterShader->setMat("projection", CaptureProjection::Projection);

			m_environmentMap->bind(0);

			for (uint32_t mip = 0; mip < ST_MAX_REFLECTION_LOD + 1; mip++)
			{
				// reisze framebuffer according to mip-level size.
				uint32_t mipWidth = static_cast<uint32_t>((m_resolution / 4) * std::pow(0.5, mip));
				uint32_t mipHeight = static_cast<uint32_t>((m_resolution / 4) * std::pow(0.5, mip));
				captureFramebuffer->resize(mipWidth, mipHeight);

				float roughness = (float)mip / (float)(ST_MAX_REFLECTION_LOD);
				prefilterShader->setFloat("roughness", roughness);
				for (uint32_t i = 0; i < 6; i++)
				{
					captureFramebuffer->attactCubeMapRefrenceTexture(prefilterMap, i, mip);

					prefilterShader->setMat("view", CaptureProjection::Views[i]);

					captureFramebuffer->bind();
					RenderCommand::clear();
					Renderer::RenderSkyBoxCube();
					captureFramebuffer->unbind();
				}
			}

			m_prefilterMap = prefilterMap;
		}
		m_brdfLUTMMap = SkyBox::genrateBRDFLUT(m_resolution);
	}

	Ref<FrameBuffer> GetCaptureBuffer(uint32_t resolution) {
		auto depthSettings = TextureSettings(TextureFormat::Depth24);
		depthSettings.wrap = TextureWrap::ClampToEdge;
		depthSettings.filtering = TextureFiltering::Nearest;

		auto captureFramebufferSpecs = FrameBufferSpecs();
		captureFramebufferSpecs.height = resolution;
		captureFramebufferSpecs.width = resolution;
		return FrameBuffer::create(captureFramebufferSpecs, TextureFormat::None, depthSettings);
	}

	Ref<Shader> SkyBox::getIrradianceShader() {
		return Renderer::getShaderSystem()->GetShader("Renderer/CubeMap/Irradiance");
	}
	Ref<Shader> SkyBox::getPrefilterShader() {
		return Renderer::getShaderSystem()->GetShader("Renderer/CubeMap/Prefilter");
	}
}