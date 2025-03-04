#include "st_pch.h"
#include "ParticleSystemLayer.h"

namespace Stulu {
	void ParticleSystemLayer::Initlize(Scene* scene) {
		m_scene = scene;
	}
	void ParticleSystemLayer::Start() {
		auto particleView = m_scene->getAllGameObjectsWith<TransformComponent, ParticleSystemComponent>();
		for (auto gameObject : particleView) {
			auto [transform, particle] = particleView.get<TransformComponent, ParticleSystemComponent>(gameObject);
			if (particle.getData().emitStart == ParticleSystemData::EmitStart::SceneStart) {
					particle.emit(transform.GetWorldPosition(), transform.GetWorldEulerRotation(), particle.getData().emitCount);
			}
		}
	}
	void ParticleSystemLayer::Update() {
		auto particleView = m_scene->getAllGameObjectsWith<TransformComponent, ParticleSystemComponent>();
		for (auto gameObject : particleView) {
			auto [transform, particle] = particleView.get<TransformComponent, ParticleSystemComponent>(gameObject);
			if (particle.getData().emitStart == ParticleSystemData::EmitStart::SceneUpdate) {
				particle.emit(transform.GetWorldPosition(), transform.GetWorldEulerRotation(), particle.getData().emitCount);
			}
			particle.update(Time::deltaTime);
		}
	}
	void ParticleSystemLayer::Render2D() {
		auto particleView = m_scene->getAllGameObjectsWith<TransformComponent, ParticleSystemComponent>();
		for (auto gameObject : particleView) {
			auto [transform, particle] = particleView.get<TransformComponent, ParticleSystemComponent>(gameObject);
			particle.draw();
		}
	}

	void ParticleSystemLayer::SerializerGameObject(YAML::Emitter& out, GameObject& gameObject) {
		BEGIN_SERIALIZE_COMPONENT(ParticleSystemComponent);
		{
			SERIALIZE_ENUMPROP(SerializedParticleSystemComponent, maxParticle);
		
			auto data = SerializedParticleSystemComponent.getData();
			SERIALIZE_ENUMPROP(data, emitStart);
			SERIALIZE_PROPERTY(data, emitCount);
			SERIALIZE_PROPERTY(data, moving);
			SERIALIZE_PROPERTY(data, speed);
			SERIALIZE_PROPERTY(data, gravity);
			SERIALIZE_PROPERTY(data, randomVelcity);
			SERIALIZE_PROPERTY(data, velocity);
			SERIALIZE_PROPERTY(data, minVelocity);
			SERIALIZE_PROPERTY(data, maxVelocity);
			SERIALIZE_PROPERTY(data, imortal);
			SERIALIZE_PROPERTY(data, lifeTime);
			SERIALIZE_PROPERTY(data, randomLifeTime);
			SERIALIZE_PROPERTY(data, randomLifeTimeMin);
			SERIALIZE_PROPERTY(data, randomLifeTimeMax);
			SERIALIZE_PROPERTY(data, rotate);
			SERIALIZE_PROPERTY(data, rotationSpeed);
			SERIALIZE_PROPERTY(data, randomRotationRange);
			SERIALIZE_PROPERTY(data, startRotation);
			SERIALIZE_PROPERTY(data, randomStartRotation);
			SERIALIZE_PROPERTY(data, randomRotationSpeed);
			SERIALIZE_PROPERTY(data, beginSize);
			SERIALIZE_PROPERTY(data, endSize);
			SERIALIZE_PROPERTY(data, randomSize);
			SERIALIZE_PROPERTY(data, beginSizeMin);
			SERIALIZE_PROPERTY(data, endSizeMin);
			SERIALIZE_PROPERTY(data, beginSizeMax);
			SERIALIZE_PROPERTY(data, endSizeMax);
			SERIALIZE_PROPERTY(data, beginColor);
			SERIALIZE_PROPERTY(data, endColor);
		}
		END_SERIALIZE_COMPONENT();
	}

	void ParticleSystemLayer::DeserializerGameObject(YAML::detail::iterator_value& gameObject, GameObject& deserialized, const std::string& path) {
		BEGIN_DESERIALIZE_COMPONENT(ParticleSystemComponent);
		{
			DESERIALIZE_ENUMPROP(AddedParticleSystemComponent, maxParticle);
			auto& data = AddedParticleSystemComponent.getData();
			DESERIALIZE_ENUMPROP(data, emitStart);
			DESERIALIZE_PROPERTY(data, emitCount);
			DESERIALIZE_PROPERTY(data, moving);
			DESERIALIZE_PROPERTY(data, speed);
			DESERIALIZE_PROPERTY(data, gravity);
			DESERIALIZE_PROPERTY(data, randomVelcity);
			DESERIALIZE_PROPERTY(data, velocity);
			DESERIALIZE_PROPERTY(data, minVelocity);
			DESERIALIZE_PROPERTY(data, maxVelocity);
			DESERIALIZE_PROPERTY(data, imortal);
			DESERIALIZE_PROPERTY(data, lifeTime);
			DESERIALIZE_PROPERTY(data, randomLifeTime);
			DESERIALIZE_PROPERTY(data, randomLifeTimeMin);
			DESERIALIZE_PROPERTY(data, randomLifeTimeMax);
			DESERIALIZE_PROPERTY(data, rotate);
			DESERIALIZE_PROPERTY(data, rotationSpeed);
			DESERIALIZE_PROPERTY(data, randomRotationRange);
			DESERIALIZE_PROPERTY(data, startRotation);
			DESERIALIZE_PROPERTY(data, randomStartRotation);
			DESERIALIZE_PROPERTY(data, randomRotationSpeed);
			DESERIALIZE_PROPERTY(data, beginSize);
			DESERIALIZE_PROPERTY(data, endSize);
			DESERIALIZE_PROPERTY(data, randomSize);
			DESERIALIZE_PROPERTY(data, beginSizeMin);
			DESERIALIZE_PROPERTY(data, endSizeMin);
			DESERIALIZE_PROPERTY(data, beginSizeMax);
			DESERIALIZE_PROPERTY(data, endSizeMax);
			DESERIALIZE_PROPERTY(data, beginColor);
			DESERIALIZE_PROPERTY(data, endColor);
		}
		END_DESERIALIZE_COMPONENT();
	}

	void ParticleSystemLayer::ClearAllParticles() {
		auto particleView = m_scene->getAllGameObjectsWith<TransformComponent, ParticleSystemComponent>();
		for (auto gameObject : particleView) {
			auto [transform, particle] = particleView.get<TransformComponent, ParticleSystemComponent>(gameObject);
			particle.clear();
		}
	}
}
