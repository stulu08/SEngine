#include "st_pch.h"
#include "OpenGLTextureCubeMap.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <Stulu/Scene/Resources.h>
#include <Stulu/Renderer/RenderCommand.h>

//https://learnopengl.com/PBR/IBL/Specular-IBL
namespace Stulu {
    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
        // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

	void OpenGLCubeMap::update(uint32_t resolution, void* data) {
        ST_PROFILING_FUNCTION();
        uint32_t m_captureFBO = 0, m_captureRBO = 0;

        if (m_envCubemap)
            glDeleteTextures(1, &m_envCubemap);
        if (m_irradianceMap)
            glDeleteTextures(1, &m_irradianceMap);
        if (m_prefilterMap)
            glDeleteTextures(1, &m_prefilterMap);
        glGenTextures(1, &m_envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
        m_resolution = resolution;
        for (uint32_t i = 0; i < 6; i++) {
            GLenum type = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
            glTexImage2D(type, 0, GL_RGB16F, m_resolution, m_resolution, 0, GL_RGB, GL_FLOAT, data);
        }
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
    void OpenGLCubeMap::update(const std::vector<std::string>& faces, uint32_t resolution) {
        ST_PROFILING_FUNCTION();
        uint32_t m_captureFBO = 0, m_captureRBO = 0;


        if (m_envCubemap)
            glDeleteTextures(1, &m_envCubemap);
        if (m_irradianceMap)
            glDeleteTextures(1, &m_irradianceMap);
        if (m_prefilterMap)
            glDeleteTextures(1, &m_prefilterMap);
        glGenTextures(1, &m_envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
        int32_t width, height, channels;
        stbi_set_flip_vertically_on_load(false);
        for (uint32_t i = 0; i < faces.size(); i++) {
            GLenum type = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
            float* textureData;
            {
                ST_PROFILING_SCOPE("reading file - OpenGLCubeMap::OpenGLCubeMap(const std::vector<std::string>&, uint32_t)");
                textureData = stbi_loadf(faces[i].c_str(), &width, &height, &channels, 0);
            }
            CORE_ASSERT(textureData, "Texture failed to load: {0}", faces[i].c_str());
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
    void OpenGLCubeMap::update(const std::string& hdrTexturePath, uint32_t resolution) {
        ST_PROFILING_FUNCTION();
        uint32_t m_captureFBO = 0, m_captureRBO = 0;

        if(m_envCubemap)
            glDeleteTextures(1, &m_envCubemap);
        if (m_irradianceMap)
            glDeleteTextures(1, &m_irradianceMap);
        if (m_prefilterMap)
            glDeleteTextures(1, &m_prefilterMap);


        m_resolution = resolution;
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
        {
            ST_PROFILING_SCOPE("reading file - OpenGLCubeMap::OpenGLCubeMap(const std::string&, uint32_t)");
            data = stbi_loadf(hdrTexturePath.c_str(), &width, &height, &nrComponents, 0);
        }
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
            CORE_ASSERT(false, "Texture failed to load: {0}", hdrTexturePath);
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
        if(s_equirectangularToCubemapShader == nullptr)
            s_equirectangularToCubemapShader = Shader::create("equirectangularToCubemapShader", getEquirectangularToCubemapShaderSource());
        s_equirectangularToCubemapShader->bind();
        s_equirectangularToCubemapShader->setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_hdrTexture);

        glViewport(0, 0, resolution, resolution); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
        for (uint32_t i = 0; i < 6; i++)
        {
            s_equirectangularToCubemapShader->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envCubemap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderCube();
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
    OpenGLCubeMap::~OpenGLCubeMap() {
		ST_PROFILING_FUNCTION();
		glDeleteTextures(1, &m_envCubemap);
		glDeleteTextures(1, &m_irradianceMap);
		glDeleteTextures(1, &m_prefilterMap);

	}
    /* generates edges wierd
    void OpenGLCubeMap::update(uint32_t resolution) {
        ST_PROFILING_FUNCTION();
        m_resolution = resolution;
        //glDeleteTextures(1, &m_envCubemap);
        glDeleteTextures(1, &m_irradianceMap);
        glDeleteTextures(1, &m_prefilterMap);
        glDeleteTextures(1, &m_brdfLUT);
        glDeleteFramebuffers(1, &m_captureFBO);
        glDeleteRenderbuffers(1, &m_captureRBO);

        glGenFramebuffers(1, &m_captureFBO);
        glGenRenderbuffers(1, &m_captureRBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_captureRBO);

        // pbr: setup cubemap to render to and attach to framebuffer
        // ---------------------------------------------------------
        uint32_t newEnviroment;
        glGenTextures(1, &newEnviroment);
        glBindTexture(GL_TEXTURE_CUBE_MAP, newEnviroment);
        for (uint32_t i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (!s_skyboxShader)
            s_skyboxShader = Shader::create("OpenGLTextureCubeMapSkyboxShader", getSkyBoxSource());
        s_skyboxShader->bind();
        s_skyboxShader->setMat4("projection", captureProjection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

        glViewport(0, 0, resolution, resolution); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
        for (uint32_t i = 0; i < 6; i++)
        {
            s_skyboxShader->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, newEnviroment, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderCube();
        }
        glDeleteTextures(1, &m_envCubemap);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_envCubemap = newEnviroment;

        // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
        glBindTexture(GL_TEXTURE_CUBE_MAP, newEnviroment);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        generateMaps();
    }
    */
    void OpenGLCubeMap::generateMaps(uint32_t m_captureFBO, uint32_t m_captureRBO) {
        ST_PROFILING_FUNCTION();
        
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
        if (!s_irradianceShader)
            s_irradianceShader = Shader::create("irradianceShader", getIrradianceShaderSource());
        s_irradianceShader->bind();
        s_irradianceShader->setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

        glViewport(0, 0, m_resolution / 16, m_resolution / 16); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
        for (uint32_t i = 0; i < 6; i++)
        {
            s_irradianceShader->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradianceMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderCube();
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
        if (!s_prefilterShader)
            s_prefilterShader = Shader::create("prefilterShader", getPrefilterShaderSorce());
        s_prefilterShader->bind();
        s_prefilterShader->setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

        glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
        uint32_t maxMipLevels = 5;
        for (uint32_t mip = 0; mip < maxMipLevels; mip++)
        {
            // reisze framebuffer according to mip-level size.
            uint32_t mipWidth = static_cast<uint32_t>((m_resolution / 4) * std::pow(0.5, mip));
            uint32_t mipHeight = static_cast<uint32_t>((m_resolution / 4) * std::pow(0.5, mip));
            glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            s_prefilterShader->setFloat("roughness", roughness);
            for (uint32_t i = 0; i < 6; i++)
            {
                s_prefilterShader->setMat4("view", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_prefilterMap, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                renderCube();
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (!s_brdfLUT) {
            // pbr: generate a 2D LUT from the BRDF equations used.
            // ----------------------------------------------------
            glGenTextures(1, &s_brdfLUT);

            // pre-allocate enough memory for the LUT texture.
            glBindTexture(GL_TEXTURE_2D, s_brdfLUT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, m_resolution, m_resolution, 0, GL_RG, GL_FLOAT, 0);
            // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
            glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
            glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_resolution, m_resolution);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_brdfLUT, 0);

            glViewport(0, 0, m_resolution, m_resolution);
            if (!s_brdfShader)
                s_brdfShader = Shader::create("brdfShader", getBrdfShaderSource());
            s_brdfShader->bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderQuad();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        bind(0);
    }

	void OpenGLCubeMap::bind(uint32_t slot) const {
		ST_PROFILING_FUNCTION();
        bindEnviromente(slot);
        bindIrradiance(slot+1);
        bindPrefilter(slot+2);
        bindBRDFLUT(slot+3);
	}
    void OpenGLCubeMap::bindEnviromente(uint32_t slot) const {
        ST_PROFILING_FUNCTION();
        glBindTextureUnit(slot, m_envCubemap);
    }
    void OpenGLCubeMap::bindIrradiance(uint32_t slot) const {
        ST_PROFILING_FUNCTION();
        glBindTextureUnit(slot, m_irradianceMap);
    }
    void OpenGLCubeMap::bindPrefilter(uint32_t slot) const {
        ST_PROFILING_FUNCTION();
        glBindTextureUnit(slot, m_prefilterMap);
    }
    void OpenGLCubeMap::bindBRDFLUT(uint32_t slot) const {
        ST_PROFILING_FUNCTION();
        glBindTextureUnit(slot, s_brdfLUT);
    }

    void OpenGLCubeMap::draw() {
        ST_PROFILING_FUNCTION();
        glDepthFunc(GL_LEQUAL);
        glCullFace(GL_BACK);
        renderCube();
        glDepthFunc(GL_LESS);
    }

	bool OpenGLCubeMap::operator==(const Texture& other) const {
		return m_envCubemap == other.getRendererID();
	}

    void OpenGLCubeMap::renderCube()
    {
        ST_PROFILING_FUNCTION();
        if (!s_cubeVAO) {
            std::vector<Vertex> vertices{
                //top
                {glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
                {glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
                {glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
                {glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
                //bottom														 		 
                {glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
                {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
                {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
                {glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
                //right															 		 
                {glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
                {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
                {glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
                {glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
                //left															 		 
                {glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
                {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
                {glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
                {glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
                //front															 		 
                {glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
                {glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},
                {glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},
                {glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},
                //back															 		 
                {glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
                {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
                {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
                {glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
            };
            std::vector<uint32_t> indices{
                //top
                0,1,2,
                2,3,0,
                //bottom
                6,5,4,
                4,7,6,
                //right
                8,9,10,
                10,11,8,
                //left
                14,13,12,
                12,15,14,
                //front
                18,17,16,
                16,19,18,
                //back
                20,21,22,
                22,23,20
            };

            
            Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
            Stulu::Ref<Stulu::IndexBuffer> indexBuffer;
            
            s_cubeVAO = Stulu::VertexArray::create();
            vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(vertices.size() * sizeof(Vertex)), &vertices[0]);
            vertexBuffer->setLayout(BufferLayout {
                { Stulu::ShaderDataType::Float3, "a_pos" },
                { Stulu::ShaderDataType::Float3, "a_normal" },
                { Stulu::ShaderDataType::Float2, "a_texCoord" },
                });
            s_cubeVAO->addVertexBuffer(vertexBuffer);
            indexBuffer = Stulu::IndexBuffer::create((uint32_t)indices.size(), indices.data());
            s_cubeVAO->setIndexBuffer(indexBuffer);
        }
        s_cubeVAO->bind();
        glDrawElements(GL_TRIANGLES, s_cubeVAO->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
    void OpenGLCubeMap::renderQuad()
    {
        ST_PROFILING_FUNCTION();
        if (s_quadVAO == 0)
        {
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &s_quadVAO);
            glGenBuffers(1, &s_quadVBO);
            glBindVertexArray(s_quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, s_quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glDisable(GL_BLEND);
        glBindVertexArray(s_quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glEnable(GL_BLEND);
    }

    std::string OpenGLCubeMap::getEquirectangularToCubemapShaderSource() {
        return R"(
            ##add ST_CubemapVertex
            ##type fragment
		    #version 460 core
            out vec4 FragColor;
            in vec3 WorldPos;

            layout (binding = 0) uniform sampler2D equirectangularMap;

            const vec2 invAtan = vec2(0.1591, 0.3183);
            vec2 SampleSphericalMap(vec3 v)
            {
                vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
                uv *= invAtan;
                uv += 0.5;
                return uv;
            }

            void main()
            {		
                vec2 uv = SampleSphericalMap(normalize(WorldPos));
                vec3 color = texture(equirectangularMap, uv).rgb;
    
                FragColor = vec4(color, 1.0);
            }
            )";
    }
    std::string OpenGLCubeMap::getIrradianceShaderSource() {
        return R"(
            ##add ST_CubemapVertex
            ##type fragment
		    #version 460 core
            out vec4 FragColor;
            in vec3 WorldPos;

            layout (binding = 0) uniform samplerCube environmentMap;

            const float PI = 3.14159265359;

            void main()
            {		
                vec3 N = normalize(WorldPos);

                vec3 irradiance = vec3(0.0);   
    
                // tangent space calculation from origin point
                vec3 up    = vec3(0.0, 1.0, 0.0);
                vec3 right = normalize(cross(up, N));
                up         = normalize(cross(N, right));
       
                float sampleDelta = 0.025;
                float nrSamples = 0.0f;
                for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
                {
                    for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
                    {
                        // spherical to cartesian (in tangent space)
                        vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
                        // tangent space to world
                        vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

                        irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
                        nrSamples++;
                    }
                }
                irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
                FragColor = vec4(irradiance, 1.0);
            }
            )";
    }
    std::string OpenGLCubeMap::getPrefilterShaderSorce() {
        return R"(
            ##add ST_CubemapVertex
            ##type fragment
		    #version 460 core
            out vec4 FragColor;
            in vec3 WorldPos;

            layout (binding = 0) uniform samplerCube environmentMap;
            uniform float roughness;

            const float PI = 3.14159265359;
            // ----------------------------------------------------------------------------
            float DistributionGGX(vec3 N, vec3 H, float roughness)
            {
                float a = roughness*roughness;
                float a2 = a*a;
                float NdotH = max(dot(N, H), 0.0);
                float NdotH2 = NdotH*NdotH;

                float nom   = a2;
                float denom = (NdotH2 * (a2 - 1.0) + 1.0);
                denom = PI * denom * denom;

                return nom / denom;
            }
            // ----------------------------------------------------------------------------
            // http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
            // efficient VanDerCorpus calculation.
            float RadicalInverse_VdC(uint bits) 
            {
                 bits = (bits << 16u) | (bits >> 16u);
                 bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
                 bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
                 bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
                 bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
                 return float(bits) * 2.3283064365386963e-10; // / 0x100000000
            }
            // ----------------------------------------------------------------------------
            vec2 Hammersley(uint i, uint N)
            {
	            return vec2(float(i)/float(N), RadicalInverse_VdC(i));
            }
            // ----------------------------------------------------------------------------
            vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
            {
	            float a = roughness*roughness;
	
	            float phi = 2.0 * PI * Xi.x;
	            float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	            float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	            // from spherical coordinates to cartesian coordinates - halfway vector
	            vec3 H;
	            H.x = cos(phi) * sinTheta;
	            H.y = sin(phi) * sinTheta;
	            H.z = cosTheta;
	
	            // from tangent-space H vector to world-space sample vector
	            vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	            vec3 tangent   = normalize(cross(up, N));
	            vec3 bitangent = cross(N, tangent);
	
	            vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	            return normalize(sampleVec);
            }
            // ----------------------------------------------------------------------------
            void main()
            {		
                vec3 N = normalize(WorldPos);
    
                // make the simplyfying assumption that V equals R equals the normal 
                vec3 R = N;
                vec3 V = R;

                const uint SAMPLE_COUNT = 1024u;
                vec3 prefilteredColor = vec3(0.0);
                float totalWeight = 0.0;
    
                for(uint i = 0u; i < SAMPLE_COUNT; i++)
                {
                    // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
                    vec2 Xi = Hammersley(i, SAMPLE_COUNT);
                    vec3 H = ImportanceSampleGGX(Xi, N, roughness);
                    vec3 L  = normalize(2.0 * dot(V, H) * H - V);

                    float NdotL = max(dot(N, L), 0.0);
                    if(NdotL > 0.0)
                    {
                        // sample from the environment's mip level based on roughness/pdf
                        float D   = DistributionGGX(N, H, roughness);
                        float NdotH = max(dot(N, H), 0.0);
                        float HdotV = max(dot(H, V), 0.0);
                        float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; 

                        float resolution = 512.0; // resolution of source cubemap (per face)
                        float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
                        float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

                        float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 
            
                        prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
                        totalWeight      += NdotL;
                    }
                }

                prefilteredColor = prefilteredColor / totalWeight;

                FragColor = vec4(prefilteredColor, 1.0);
            }
            )";
    }
    std::string OpenGLCubeMap::getSkyBoxSource() {
        return R"(
            ##add ST_CubemapVertex
            ##type fragment
		    #version 460 core
            out vec4 FragColor;
            in vec3 WorldPos;
            layout (binding = 0) uniform samplerCube environmentMap;
            void main()
            {		
                FragColor = vec4(texture(environmentMap, WorldPos).rgb, 1.0);
            }
            )";
    }
    std::string OpenGLCubeMap::getBrdfShaderSource() {
        return R"(
            ##type vertex
#version 440 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;

void main() {
    TexCoords = texCoords;
	gl_Position = vec4(position, 1.0);
}
##type fragment
#version 440 core
in vec2 TexCoords;
out vec2 FragColor;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
// ----------------------------------------------------------------------------
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
    
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
    
    // from spherical coordinates to cartesian coordinates - halfway vector
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
    
    // from tangent-space H vector to world-space sample vector
    vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    // note that we use a different k for IBL
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec2 IntegrateBRDF(float NdotV, float roughness)
{
    vec3 V;
    V.x = sqrt(1.0 - NdotV*NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0; 

    vec3 N = vec3(0.0, 0.0, 1.0);
    
    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL > 0.0)
        {
            float G = GeometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}
// ----------------------------------------------------------------------------
void main() {
    vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);
    FragColor = integratedBRDF;
}
        )";
    }
}