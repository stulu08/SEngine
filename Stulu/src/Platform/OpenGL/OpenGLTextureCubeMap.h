#pragma once
#include "Stulu/Renderer/Texture.h"
#include <Stulu/Renderer/Shader.h>
#include "OpenGLVertexArray.h"
namespace Stulu {
	class STULU_API OpenGLCubeMap : public CubeMap {
	public:
		OpenGLCubeMap(uint32_t resolution, TextureSettings settings);

		virtual ~OpenGLCubeMap();

		virtual void bind(uint32_t slot) const override;
		virtual void draw() override;

		virtual uint32_t getRendererID() const override { return m_map; }
		virtual uint32_t getMap() const override { return m_map; }
		virtual uint32_t getWidth() const override { return m_resolution; }
		virtual uint32_t getHeight() const override { return m_resolution; }

		virtual bool operator == (const Texture& other) const override;
		virtual operator int() override { return m_map; }
	private:
		uint32_t m_resolution;
		uint32_t m_map = 0;
	};
	class STULU_API OpenGLSkyBox : public SkyBox {
	public:
		OpenGLSkyBox(uint32_t resolution, void* data) { update(resolution, data); }
		OpenGLSkyBox(const std::vector<std::string>& faces, uint32_t resolution) { update(faces, resolution); }
		OpenGLSkyBox(const std::string& hdrTexturePath, uint32_t resolution) { update(hdrTexturePath, resolution); }

		virtual ~OpenGLSkyBox();

		virtual void bind(uint32_t slot) const override;
		virtual void bindEnviromente(uint32_t slot) const override;
		virtual void bindIrradiance(uint32_t slot) const override;
		virtual void bindPrefilter(uint32_t slot) const override;
		virtual void bindBRDFLUT(uint32_t slot) const override;

		virtual void draw() override;

		virtual uint32_t getRendererID() const override { return m_envCubemap; }

		virtual uint32_t getMap() const override { return m_envCubemap; }
		virtual uint32_t getEnviroment() const override { return m_envCubemap; }
		virtual uint32_t getIrradianceMap() const override { return m_irradianceMap; }
		virtual uint32_t getPrefilterMap() const override { return m_prefilterMap; }
		virtual uint32_t getBRDFLUT() const override { return s_brdfLUT; }

		virtual uint32_t getWidth() const override { return m_resolution; }
		virtual uint32_t getHeight() const override { return m_resolution; }

		virtual bool operator == (const Texture& other) const override;
		virtual operator int() override { return m_envCubemap; }

		virtual void update(uint32_t resolution, void* data) override;
		virtual void update(const std::vector<std::string>& faces, uint32_t resolution) override;
		virtual void update(const std::string& hdrTexturePath, uint32_t resolution) override;
	private:
		uint32_t m_resolution;//need to be set in constructor
		uint32_t m_envCubemap;//need to be set in constructor
		uint32_t m_irradianceMap = 0, m_prefilterMap = 0;
		static inline uint32_t s_brdfLUT = 0;
		static inline uint32_t s_quadVAO = 0, s_quadVBO = 0;
		static inline Ref<Shader> s_brdfShader = nullptr, s_prefilterShader = nullptr, s_irradianceShader = nullptr, s_equirectangularToCubemapShader = nullptr, s_skyboxShader = nullptr;

		void generateMaps(uint32_t m_captureFBO, uint32_t m_captureRBO);

		void renderQuad();

		static std::string getEquirectangularToCubemapShaderSource();
		static std::string getIrradianceShaderSource();
		static std::string getPrefilterShaderSorce();
		static std::string getSkyBoxSource();
		static std::string getBrdfShaderSource();
	};
}