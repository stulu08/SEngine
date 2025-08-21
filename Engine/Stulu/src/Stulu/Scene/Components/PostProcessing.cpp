#include "st_pch.h"
#include "PostProcessing.h"

#include "Stulu/Renderer/PostProcessing/Bloom.h"

namespace Stulu {
	PostProcessingComponent::PostProcessingComponent() {
		m_effects.push_back(createRef<GammaCorrectionEffect>());
	}
	PostProcessingComponent::PostProcessingComponent(const PostProcessingComponent& other) {
		for (const Ref<PostProcessingEffect>& sourceEffect : other.m_effects) {
			m_effects.push_back(sourceEffect->CreateCopy());
		}
	}

	bool PostProcessingComponent::HasEffect(uint32_t id) const {
		for (const auto& effect : m_effects) {
			if (effect->EffectID() == id)
				return true;
		}
		return false;
	}
	PostProcessingEffect* PostProcessingComponent::GetEffect(uint32_t id) const {
		for (const auto& effect : m_effects) {
			if (effect->EffectID() == id)
				return effect.get();
		}
		return nullptr;
	}
	PostProcessingEffect* PostProcessingComponent::AddEffect(Ref<PostProcessingEffect> effect) {
		if (HasEffect(effect->EffectID())) {
			CORE_ERROR("Component already has PostProcessingEffect!");
		}
		else {
			m_effects.push_back(effect);
		}

		// sort from lowest to highest
		std::sort(m_effects.begin(), m_effects.end(),
			[](const auto& left, const auto& right) {
				return left->EffectID() < right->EffectID();
			});

		return GetEffect(effect->EffectID());
	}
	void PostProcessingComponent::RemoveEffect(uint32_t id) {
		for (size_t i = 0; i < m_effects.size(); i++) {
			if (m_effects[i]->EffectID() == id) {
				m_effects.erase(m_effects.begin() + i);
				return;
			}
		}
	}
	void PostProcessingComponent::Serialize(YAML::Emitter& out) const {

		if (HasEffect<GammaCorrectionEffect>()) {
			const GammaCorrectionEffect& gammaEffect = *GetEffect<GammaCorrectionEffect>();

			out << YAML::Key << "Gamma Correction" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "enabled" << YAML::Value << gammaEffect.IsEnabled();
			out << YAML::Key << "gamma" << YAML::Value << gammaEffect.gamma;
			out << YAML::Key << "exposure" << YAML::Value << gammaEffect.exposure;
			out << YAML::Key << "maxWhite" << YAML::Value << gammaEffect.maxWhite;
			out << YAML::Key << "mappingMode" << YAML::Value << (uint32_t)gammaEffect.toneMappingMode;
			out << YAML::EndMap;
		}

		if (HasEffect<BloomEffect>()) {
			const BloomEffect& bloom = *GetEffect<BloomEffect>();

			out << YAML::Key << "Bloom" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "enabled" << YAML::Value << bloom.IsEnabled();
			out << YAML::Key << "intensity" << YAML::Value << bloom.intensity;
			out << YAML::Key << "threshold" << YAML::Value << bloom.threshold;
			out << YAML::Key << "knee" << YAML::Value << bloom.knee;
			out << YAML::Key << "clamp" << YAML::Value << bloom.clamp;
			out << YAML::Key << "resolutionScale" << YAML::Value << bloom.resolutionScale;
			out << YAML::Key << "diffusion" << YAML::Value << bloom.diffusion;
			out << YAML::Key << "maxSamples" << YAML::Value << bloom.maxSamples;
			out << YAML::EndMap;
		}
	}

	void PostProcessingComponent::Deserialize(YAML::Node& node) {
		if (node["Gamma Correction"]) {
			GammaCorrectionEffect& gammaCorrection = HasEffect<GammaCorrectionEffect>() ?
				*GetEffect<GammaCorrectionEffect>() : *AddEffect<GammaCorrectionEffect>();

			auto gammaCorrectionNode = node["Gamma Correction"];

			if (gammaCorrectionNode["enabled"])
				gammaCorrection.SetEnabled(gammaCorrectionNode["enabled"].as<bool>());
			if (gammaCorrectionNode["gamma"])
				gammaCorrection.gamma = gammaCorrectionNode["gamma"].as<float>();
			if (gammaCorrectionNode["exposure"])
				gammaCorrection.exposure = gammaCorrectionNode["exposure"].as<float>();
			if (gammaCorrectionNode["maxWhite"])
				gammaCorrection.maxWhite = gammaCorrectionNode["maxWhite"].as<float>();
			if (gammaCorrectionNode["mappingMode"])
				gammaCorrection.toneMappingMode = (GammaCorrectionEffect::ToneMappingMode)gammaCorrectionNode["mappingMode"].as<uint32_t>();
		}
		if (node["Bloom"]) {
			auto bloomNode = node["Bloom"];
			BloomEffect& bloom = HasEffect<BloomEffect>() ?
				*GetEffect<BloomEffect>() : *AddEffect<BloomEffect>();

			if (bloomNode["enabled"])
				bloom.SetEnabled(bloomNode["enabled"].as<bool>());
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
				bloom.diffusion = bloomNode["diffusion"].as<float>();
			if (bloomNode["maxSamples"])
				bloom.maxSamples = bloomNode["maxSamples"].as<uint32_t>();
		}
	}
}
