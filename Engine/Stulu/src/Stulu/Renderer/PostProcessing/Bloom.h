#pragma once
#include "Effect.h"

namespace Stulu {

	constexpr auto BLOOM_MAX_SAMPLES = 10;
	constexpr auto BLOOM_MIN_WIDTH = 7;
	constexpr auto BLOOM_MIN_HEIGHT = 5;

	class STULU_API BloomEffect : public PostProcessingEffect {
	public:
		BloomEffect();
		BloomEffect(const BloomEffect&) = default;
		virtual ~BloomEffect() = default;

		virtual void Apply(Ref<FrameBuffer> destination, const Texture2D* source) override;

		virtual Ref<PostProcessingEffect> CreateCopy() const override {
			return createRef<BloomEffect>(*this);
		}

		inline static uint32_t STATIC_ID() { return PFX_ID_BLOOM; }
		virtual uint32_t EffectID() const override { return STATIC_ID(); }
	public:
		float intensity = .04f;
		float threshold = 5.0f;
		float knee = 0.45f;
		float clamp = 15.0f;
		float resolutionScale = 0.5f;
		float diffusion = 7;
		uint32_t maxSamples = 10;
	private:
		Ref<Shader> m_shader;
		Ref<Shader> m_upSampleShader;
		Ref<Shader> m_downSampleShader;
		Ref<Shader> m_filterShader;

		Ref<Texture2D> downSample = nullptr;
		Ref<Texture2D> upSample = nullptr;
		uint32_t samples = 0;
		float sampleScale = 0;

		Texture2D* CreateBloom(const Texture2D* sourceTex);
	};
}