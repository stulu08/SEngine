#pragma once
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	constexpr auto PFX_ID_BLOOM = 100;
	constexpr auto PFX_ID_GAMMA = 200;


	class STULU_API PostProcessingEffect {
	public:
		virtual ~PostProcessingEffect() = default;
		virtual Ref<PostProcessingEffect> CreateCopy() const = 0;

		virtual void Apply(Ref<FrameBuffer> destination, const Texture2D* source) = 0;

		// unique id for the effect, also priority sorting, effect with the smalles id will be applied first
		virtual uint32_t EffectID() const = 0;

		inline bool IsEnabled() const { return m_enabled; }
		inline void SetEnabled(bool value) { m_enabled = value; }
	private:
		bool m_enabled = true;
	};

	class STULU_API GammaCorrectionEffect : public PostProcessingEffect {
	public:
		enum ToneMappingMode {
			Disabled = 0,
			Reinhard = 1,
			ReinhardExtended = 2,
			Aces = 3,
		};

		GammaCorrectionEffect() {
			m_shader = Renderer::getShaderSystem()->GetShader("Renderer/PostProcessing/GammaCorrection");
		}
		GammaCorrectionEffect(const GammaCorrectionEffect&) = default;
		~GammaCorrectionEffect() = default;


		virtual void Apply(Ref<FrameBuffer> destination, const Texture2D* source) override;

		virtual Ref<PostProcessingEffect> CreateCopy() const override{
			return createRef<GammaCorrectionEffect>(*this);
		}

		inline static uint32_t STATIC_ID() { return PFX_ID_GAMMA; }
		virtual uint32_t EffectID() const override { return STATIC_ID(); }
	public:
		float exposure = 1.0f;
		float maxWhite = 4.0f;
		bool enableGammaCorrection = true;
		ToneMappingMode toneMappingMode = ToneMappingMode::Reinhard;
		float gamma = 2.2f;
	private:
		Ref<Shader> m_shader;
	};
}