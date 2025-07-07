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

	class Effect {

	};

	class PostProcessingComponent : public Component {
	public:
		PostProcessingComponent() {
			data = PostProcessingData();
		}
		PostProcessingComponent(const PostProcessingComponent&) = default;

		PostProcessingData data;

		virtual void Serialize(YAML::Emitter& out) const override {

			out << YAML::Key << "Gamma Correction" << YAML::Value << YAML::BeginMap;
			{
				const auto& gammaCorrection = data.settings.gammaCorrection;
				out << YAML::Key << "enabled" << YAML::Value << gammaCorrection.enabled;;
				out << YAML::Key << "gamma" << YAML::Value << gammaCorrection.gamma;;
				out << YAML::Key << "exposure" << YAML::Value << gammaCorrection.exposure;;
			}
			out << YAML::EndMap;

			out << YAML::Key << "Bloom" << YAML::Value << YAML::BeginMap;
			{
				const auto& bloom = data.settings.bloom;
				out << YAML::Key << "enabled" << YAML::Value << bloom.enabled;;
				out << YAML::Key << "intensity" << YAML::Value << bloom.intensity;;
				out << YAML::Key << "threshold" << YAML::Value << bloom.threshold;;
				out << YAML::Key << "knee" << YAML::Value << bloom.knee;;
				out << YAML::Key << "clamp" << YAML::Value << bloom.clamp;;
				out << YAML::Key << "resolutionScale" << YAML::Value << bloom.resolutionScale;;
				out << YAML::Key << "diffusion" << YAML::Value << bloom.diffusion;;
				out << YAML::Key << "maxSamples" << YAML::Value << bloom.maxSamples;;
			}
			out << YAML::EndMap;
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["Gamma Correction"]) {
				auto gammaCorrectionNode = node["Gamma Correction"];
				auto& gammaCorrection = data.settings.gammaCorrection;

				if (gammaCorrectionNode["enabled"])
					gammaCorrection.enabled = gammaCorrectionNode["enabled"].as<bool>();
				if (gammaCorrectionNode["gamma"])
					gammaCorrection.gamma = gammaCorrectionNode["gamma"].as<float>();
				if (gammaCorrectionNode["exposure"])
					gammaCorrection.exposure = gammaCorrectionNode["exposure"].as<float>();
			}
			if (node["Bloom"]) {
				auto bloomNode = node["Bloom"];
				auto& bloom = data.settings.bloom;

				if (bloomNode["enabled"])
					bloom.enabled = bloomNode["enabled"].as<bool>();
				if (bloomNode["intensity"])
					bloom.intensity = bloomNode["intensity"].as<float>();
				if (bloomNode["threshold"])
					bloom.threshold = bloomNode["threshold"].as<float>();
				if (bloomNode["knee"])
					bloom.knee = bloomNode["knee"].as<float>();
				if (bloomNode["clamp"])
					bloom.clamp = bloomNode["clamp"].as<float>();
				if (bloomNode["resolutionScale"])
					bloom.resolutionScale = bloomNode["resolutionScale"].as<float>();
				if (bloomNode["diffusion"])
					bloom.diffusion = bloomNode["diffusion"].as<uint32_t>();
				if (bloomNode["maxSamples"])
					bloom.maxSamples = bloomNode["maxSamples"].as<uint32_t>();
			}
		}
	};
}