#include "st_pch.h"
#include "OpenGLTextureCubeMap.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <Stulu/Resources/Resources.h>
#include <Stulu/Renderer/RenderCommand.h>
#include <Stulu/Renderer/Renderer.h>
#include "OpenGlTexture.h"
#include "OpenGLStateCache.h"

//https://learnopengl.com/PBR/IBL/Specular-IBL
namespace Stulu {

	static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	static glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	OpenGLSkyBox::OpenGLSkyBox(const std::vector<std::string>& faces, uint32_t resolution)
		: m_resolution(resolution) {

		uint32_t m_captureFBO = 0, m_captureRBO = 0;

		glGenTextures(1, &m_envCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
		int32_t width, height, channels;
		stbi_set_flip_vertically_on_load(false);
		for (uint32_t i = 0; i < faces.size(); i++) {
			GLenum type = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
			float* textureData;
			textureData = stbi_loadf(faces[i].c_str(), &width, &height, &channels, 0);

			CORE_ASSERT(textureData, std::string("Texture failed to load: ") + faces[i]);
			glTexImage2D(type, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, textureData);
			stbi_image_free(textureData);
		}
		m_resolution = resolution;
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenFramebuffers(1, &m_captureFBO);
		glGenRenderbuffers(1, &m_captureRBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_resolution, m_resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_captureRBO);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, m_resolution, m_resolution); // don't forget to configure the viewport to the capture dimensions.
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		generateMaps(m_captureFBO, m_captureRBO);

		glDeleteFramebuffers(1, &m_captureFBO);
		glDeleteRenderbuffers(1, &m_captureRBO);
	}

	OpenGLSkyBox::OpenGLSkyBox(const std::string& hdrTexturePath, uint32_t resolution)
		: m_resolution(resolution) {
		uint32_t m_captureFBO = 0, m_captureRBO = 0;

		glGenFramebuffers(1, &m_captureFBO);
		glGenRenderbuffers(1, &m_captureRBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_captureRBO);

		// pbr: load the HDR environment map
		// ---------------------------------
		uint32_t m_hdrTexture;
		stbi_set_flip_vertically_on_load(true);
		int32_t width, height, nrComponents;
		float* data;
		data = stbi_loadf(hdrTexturePath.c_str(), &width, &height, &nrComponents, 0);

		if (data)
		{
			glGenTextures(1, &m_hdrTexture);
			glBindTexture(GL_TEXTURE_2D, m_hdrTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			CORE_ASSERT(false, std::string("Texture failed to load: ") + hdrTexturePath);
		}

		// pbr: setup cubemap to render to and attach to framebuffer
		// ---------------------------------------------------------
		glGenTextures(1, &m_envCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
		for (uint32_t i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// pbr: convert HDR equirectangular environment map to cubemap equivalent
		// ----------------------------------------------------------------------
		auto equirectangularToCubemapShader = getEquirectangularToCubemapShader();
		equirectangularToCubemapShader->bind();
		equirectangularToCubemapShader->setMat("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_hdrTexture);

		glViewport(0, 0, resolution, resolution); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
		for (uint32_t i = 0; i < 6; i++)
		{
			equirectangularToCubemapShader->setMat("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envCubemap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer::RenderSkyBoxCube();
		}
		glDeleteTextures(1, &m_hdrTexture);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		generateMaps(m_captureFBO, m_captureRBO);
		glDeleteFramebuffers(1, &m_captureFBO);
		glDeleteRenderbuffers(1, &m_captureRBO);
	}

	OpenGLSkyBox::~OpenGLSkyBox() {
		glDeleteTextures(1, &m_envCubemap);
		glDeleteTextures(1, &m_irradianceMap);
		glDeleteTextures(1, &m_prefilterMap);

	}
	void OpenGLSkyBox::generateMaps(uint32_t m_captureFBO, uint32_t m_captureRBO) {
		m_settings.wrap = TextureWrap::ClampToEdge;
		m_settings.tiling = glm::vec2(1.0f);
		m_settings.format = TextureFormat::RGB16F;
		m_settings.filtering = TextureFiltering::Trilinear;
		// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
		// --------------------------------------------------------------------------------
		glGenTextures(1, &m_irradianceMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
		for (uint32_t i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_resolution / 16, m_resolution / 16, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_resolution / 16, m_resolution / 16);

		// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
		// -----------------------------------------------------------------------------
		auto irradianceShader = getIrradianceShader();
		irradianceShader->bind();
		irradianceShader->setMat("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

		glViewport(0, 0, m_resolution / 16, m_resolution / 16); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
		for (uint32_t i = 0; i < 6; i++)
		{
			irradianceShader->setMat("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradianceMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer::RenderSkyBoxCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
		// --------------------------------------------------------------------------------
		glGenTextures(1, &m_prefilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);
		for (uint32_t i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_resolution / 4, m_resolution / 4, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
		// ----------------------------------------------------------------------------------------------------
		auto prefilterShader = getPrefilterShader();
		prefilterShader->bind();
		prefilterShader->setMat("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

		glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
		for (uint32_t mip = 0; mip < ST_MAX_REFLECTION_LOD + 1; mip++)
		{
			// reisze framebuffer according to mip-level size.
			uint32_t mipWidth = static_cast<uint32_t>((m_resolution / 4) * std::pow(0.5, mip));
			uint32_t mipHeight = static_cast<uint32_t>((m_resolution / 4) * std::pow(0.5, mip));
			glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(ST_MAX_REFLECTION_LOD);
			prefilterShader->setFloat("roughness", roughness);
			for (uint32_t i = 0; i < 6; i++)
			{
				prefilterShader->setMat("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_prefilterMap, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				Renderer::RenderSkyBoxCube();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		m_brdfLUT = (uint32_t)*SkyBox::genrateBRDFLUT(m_resolution);

		bind(0);
	}

	void OpenGLSkyBox::bind(uint32_t slot) const {
		bindEnviromente(slot + ST_SKYBOX_TEXTURE_BIND_ENV_OFF);
		bindIrradiance(slot + ST_SKYBOX_TEXTURE_BIND_IRR_OFF);
		bindPrefilter(slot + ST_SKYBOX_TEXTURE_BIND_PRE_OFF);
		bindBRDFLUT(slot + ST_SKYBOX_TEXTURE_BIND_BRD_OFF);
	}
	void OpenGLSkyBox::bindEnviromente(uint32_t slot) const {
		OpenGLStateCache::BindTextureUnit(slot, m_envCubemap);
	}
	void OpenGLSkyBox::bindIrradiance(uint32_t slot) const {
		OpenGLStateCache::BindTextureUnit(slot, m_irradianceMap);
	}
	void OpenGLSkyBox::bindPrefilter(uint32_t slot) const {
		OpenGLStateCache::BindTextureUnit(slot, m_prefilterMap);
	}
	void OpenGLSkyBox::bindBRDFLUT(uint32_t slot) const {
		OpenGLStateCache::BindTextureUnit(slot, m_brdfLUT);
	}

	bool OpenGLSkyBox::operator==(const Texture& other) const {
		return m_envCubemap == *static_cast<uint32_t*>(other.getNativeRendererObject());
	}

	Ref<Shader> OpenGLSkyBox::getEquirectangularToCubemapShader() {
		Ref<Shader> shader;
		if (!shader) {
			shader = Renderer::getShaderSystem()->GetShader("Renderer/CubeMap/EquirectangularToCubemap");
		}
		return shader;
	}
	Ref<Shader> OpenGLSkyBox::getIrradianceShader() {
		Ref<Shader> shader;
		if (!shader) {
			shader = Renderer::getShaderSystem()->GetShader("Renderer/CubeMap/Irradiance");
		}
		return shader;
	}
	Ref<Shader> OpenGLSkyBox::getPrefilterShader() {
		Ref<Shader> shader;
		if (!shader) {
			shader = Renderer::getShaderSystem()->GetShader("Renderer/CubeMap/Prefilter");
		}
		return shader;
	}

	OpenGLCubeMap::OpenGLCubeMap(uint32_t resolution, TextureSettings settings)
		: m_resolution(resolution), m_settings(settings) {
		glGenTextures(1, &m_map);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_map);

		bool hasMips = m_settings.filtering == TextureFiltering::Linear || m_settings.levels > 1;

		auto [internalformat, dataformat] = TextureFormatToGLenum(m_settings.format);


		for (uint32_t i = 0; i < 6; i++)
		{
			if (isGLTextureFormatFloat(m_settings.format))
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, m_resolution, m_resolution, 0, dataformat, GL_FLOAT, nullptr);
			else
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, m_resolution, m_resolution, 0, dataformat, GL_UNSIGNED_BYTE, nullptr);
		}
		GLenum wrap = TextureWrapToGLenum(m_settings.wrap);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, TextureFilteringToGLenumMinification(m_settings.filtering));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, TextureFilteringToGLenumMagnification(m_settings.filtering));

		if (hasMips)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	OpenGLCubeMap::~OpenGLCubeMap() {
		glDeleteTextures(1, &m_map);
	}

	void OpenGLCubeMap::bind(uint32_t slot) const {
		OpenGLStateCache::BindTextureUnit(slot, m_map);
	}

	void OpenGLCubeMap::genMips() const {
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_map);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	bool OpenGLCubeMap::operator==(const Texture& other) const {
		return m_map == *static_cast<uint32_t*>(other.getNativeRendererObject());
	}
}