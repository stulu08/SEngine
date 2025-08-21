#pragma once
#include "ParticleSystem.h"

namespace Stulu {
	class ParticleSystemLayer : public SceneLayer {
	public:
		~ParticleSystemLayer() = default;

		virtual bool Initlize(Scene* scene) override;

		virtual void SceneStart() override;
		virtual void Update() override;

		virtual void Render2D() override;
	private:
		Scene* m_scene;
	
		void ClearAllParticles();
	};
}