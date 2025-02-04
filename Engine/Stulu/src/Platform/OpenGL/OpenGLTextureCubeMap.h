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
		virtual void draw() const override;
		virtual void genMips() const override;

		virtual void* getNativeRendererObject() const override { return (void*)(&m_map); }
		virtual void* getMap() const override { return getNativeRendererObject(); }
		virtual uint32_t getWidth() const override { return m_resolution; }
		virtual uint32_t getHeight() const override { return m_resolution; }
		virtual TextureSettings& getSettings() override { return m_settings; }

		virtual bool operator == (const Texture& other) const override;
		virtual operator int() override { return m_map; }
	private:
		uint32_t m_resolution;
		uint32_t m_map = 0;
		TextureSettings m_settings;
		friend class OpenGLReflectionMap;
	};
	class STULU_API OpenGLReflectionMap : public ReflectionMap {
	public:
		OpenGLReflectionMap(uint32_t resolution, TextureSettings settings = TextureSettings(TextureFormat::RGB16F, TextureWrap::ClampToEdge, {1,1}, ST_MAX_REFLECTION_LOD+1));
		virtual ~OpenGLReflectionMap();

		virtual void bind(uint32_t slot) const override { m_prefilterMap->bind(slot); }
		virtual void draw() const override { m_sourceMap->draw(); }
		virtual void genMips() const override { m_sourceMap->genMips(); }

		virtual void GenerateReflectionMips(uint32_t) override;

		virtual void* getNativeRendererObject() const override { return m_sourceMap->getNativeRendererObject(); }
		virtual void* getMap() const override { return getNativeRendererObject(); }
		virtual uint32_t getWidth() const override { return m_sourceMap->getWidth(); }
		virtual uint32_t getHeight() const override { return m_sourceMap->getHeight(); }
		virtual TextureSettings& getSettings() override { return m_sourceMap->getSettings(); }

		virtual bool operator== (const Texture& other) const override;
		virtual operator int() override { return m_sourceMap->m_map; }
	private:
		Scope<OpenGLCubeMap> m_sourceMap;
		Ref<OpenGLCubeMap> m_prefilterMap;
		uint32_t m_captureFBO, m_captureRBO;
	};
	class OpenGLSkyBox : public SkyBox {
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

		virtual void draw() const override;
		virtual void genMips() const override { CORE_WARN("Not supported"); }

		virtual void* getNativeRendererObject() const override { return (void*)(&m_envCubemap); }

		virtual void* getMap() const override { return (void*)(&m_envCubemap);; }
		virtual void* getEnviroment() const override { return (void*)(&m_envCubemap);; }
		virtual void* getIrradianceMap() const override { return (void*)(&m_irradianceMap);; }
		virtual void* getPrefilterMap() const override { return (void*)(&m_prefilterMap);; }
		virtual void* getBRDFLUT() const override { return (void*)(&m_brdfLUT);; }

		virtual uint32_t getWidth() const override { return m_resolution; }
		virtual uint32_t getHeight() const override { return m_resolution; }
		virtual TextureSettings& getSettings() override { return m_settings; }

		virtual bool operator == (const Texture& other) const override;
		virtual operator int() override { return m_envCubemap; }

		virtual void update(uint32_t resolution, void* data) override;
		virtual void update(const std::vector<std::string>& faces, uint32_t resolution) override;
		virtual void update(const std::string& hdrTexturePath, uint32_t resolution) override;

		static Ref<Texture2D> genrateBRDFLUT(uint32_t resolution);
	private:
		TextureSettings m_settings;
		uint32_t m_resolution;//need to be set in constructor
		uint32_t m_envCubemap;//need to be set in constructor
		uint32_t m_irradianceMap = 0, m_prefilterMap = 0;
		uint32_t m_brdfLUT = 0;
		static inline uint32_t s_quadVAO = 0, s_quadVBO = 0;

		void generateMaps(uint32_t m_captureFBO, uint32_t m_captureRBO);

		static void renderQuad();

		friend class OpenGLReflectionMap;

		static Ref<Shader> getEquirectangularToCubemapShader();
		static Ref<Shader> getIrradianceShader();
		static Ref<Shader> getPrefilterShader();
		static Ref<Shader> getBRDFLUTShader();
	};
}