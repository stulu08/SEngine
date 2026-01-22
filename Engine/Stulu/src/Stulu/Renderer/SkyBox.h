#pragma once
#include "Stulu/Renderer/Texture.h"
#include "Stulu/Renderer/FrameBuffer.h"
#include "Stulu/Renderer/Shader.h"

namespace Stulu {
	class STULU_API SkyBox : public CubeMap {
	public:
		static Ref<SkyBox> Create(uint32_t resolution, const std::string& path, const TextureSettings settings = TextureSettings(TextureFormat::Auto, TextureWrap::ClampToEdge, TextureFiltering::Trilinear));
		static Ref<SkyBox> CreateFromCubeMap(Ref<CubeMap> cubeMap);

		static Ref<Texture2D> genrateBRDFLUT(uint32_t resolution);

		virtual ~SkyBox() = default;

		virtual void bind(uint32_t slot) const {
			bindEnviromente(slot + ST_SKYBOX_TEXTURE_BIND_ENV_OFF);
			bindIrradiance(slot + ST_SKYBOX_TEXTURE_BIND_IRR_OFF);
			bindPrefilter(slot + ST_SKYBOX_TEXTURE_BIND_PRE_OFF);
			bindBRDFLUT(slot + ST_SKYBOX_TEXTURE_BIND_BRD_OFF);
		}
		virtual void bindEnviromente(uint32_t slot) const {
			m_environmentMap->bind(slot);
		}
		virtual void bindIrradiance(uint32_t slot) const {
			m_irradianceMap->bind(slot);
		}
		virtual void bindPrefilter(uint32_t slot) const {
			m_prefilterMap->bind(slot);
		}
		virtual void bindBRDFLUT(uint32_t slot) const {
			m_brdfLUTMMap->bind(slot);
		}

		virtual void GenerateMips() const override { CORE_WARN("Not supported"); }

		virtual void* getNativeRendererObject() const override { return getEnviroment(); }

		virtual void* getEnviroment() const { return m_environmentMap->getNativeRendererObject(); }
		virtual void* getIrradianceMap() const { return m_irradianceMap->getNativeRendererObject(); }
		virtual void* getPrefilterMap() const { return m_prefilterMap->getNativeRendererObject(); }
		virtual void* getBRDFLUT() const { return m_brdfLUTMMap->getNativeRendererObject(); }

		virtual uint32_t getWidth() const override { return m_resolution; }
		virtual uint32_t getHeight() const override { return m_resolution; }
		virtual TextureSettings& getSettings() override { return m_settings; }

		virtual bool operator==(const Texture& other) const override {
			return NativeRenderObjectCast<uint32_t>(getNativeRendererObject()) == NativeRenderObjectCast<uint32_t>(other.getNativeRendererObject());
		}
		virtual operator int() override { return m_environmentMap->operator int(); }

		SkyBox(const Ref<CubeMap> cubeMap);
	private:

		TextureSettings m_settings;
		uint32_t m_resolution;
		Ref<CubeMap> m_irradianceMap, m_prefilterMap, m_environmentMap;
		Ref<Texture2D> m_brdfLUTMMap;

		void generateMaps();

		friend class OpenGLReflectionMap;

		static Ref<Shader> getIrradianceShader();
		static Ref<Shader> getPrefilterShader();
	};
}