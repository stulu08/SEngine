#pragma once
#include "ParticleSystem.h"

namespace Stulu {
	class ParticleSystemLayer : public SceneLayer {
	public:
		~ParticleSystemLayer() = default;

		virtual void Initlize(Scene* scene) override;

		virtual void Start() override;
		virtual void Update() override;

		virtual void Render2D() override;

		virtual void SerializerGameObject(YAML::Emitter& out, GameObject& gameObject) override;
		virtual void DeserializerGameObject(YAML::detail::iterator_value& gameObject, GameObject& deserialized, const std::string& path) override;
	private:
		Scene* m_scene;
	
		void ClearAllParticles();
	};
}