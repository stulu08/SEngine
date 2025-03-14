#pragma once
#include "Component.h"

namespace Stulu {
	#define BLOOM_MAX_SAMPLES 10

	struct PostProcessingSettings {
		struct {
			bool enabled = true;
			float gamma = 2.2f;
			float exposure = 1.0f;
		} gammaCorrection;

		struct {
			bool enabled = false;
			float intensity = .04f;
			float threshold = 5.0f;
			float knee = 0.45f;
			float clamp = 15.0f;
			float resolutionScale = 1.0f / 2.0f;
			uint32_t diffusion = 7;
			uint32_t maxSamples = 10;
		} bloom;

	};

	struct PostProcessingData {
		PostProcessingSettings settings;

		struct {
			Ref<Texture2D> downSample;
			Ref<Texture2D> upSample;
			uint32_t samples = 0;
			float sampleScale = 0;

		} bloom;
	};

	class PostProcessingComponent : public Component {
	public:
		PostProcessingComponent() {
			data = PostProcessingData();
		}
		PostProcessingComponent(const PostProcessingComponent&) = default;

		PostProcessingData data;
	};
}