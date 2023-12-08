#include "st_pch.h"
#include "ParticleSystem.h"
#include "Stulu/Renderer/Renderer2D.h"

namespace Stulu {
	void ParticleSystemComponent::emit(const glm::vec3& pos, const glm::vec3 rotation, uint32_t count) {
		for (size_t i = 0; i < count; i++) {
			if (maxParticle > particles.size()) {
				Particle part = Particle();
				if (data.randomLifeTime)
					part.dieTime = Stulu::Random::getFloat(data.randomLifeTimeMin, data.randomLifeTimeMax);
				else
					part.dieTime = data.lifeTime;
				if (data.randomSize) {
					part.beginSize = Stulu::Random::getVector3(data.beginSizeMin, data.beginSizeMax);
					part.endSize = Stulu::Random::getVector3(data.endSizeMin, data.endSizeMax);
				}
				else {
					part.beginSize = data.beginSize;
					part.endSize = data.endSize;
				}
				if (data.randomVelcity)
					part.velocity = Stulu::Random::getVector3(data.minVelocity, data.maxVelocity);

				part.beginColor = data.beginColor;
				part.endColor = data.endColor;

				part.lifeTime = .0f;

				part.pos = pos;
				part.rotation = rotation;
				if (data.randomStartRotation)
					part.rotation.z = Stulu::Random::getFloat(.0f, 360.0f);
				else
					part.rotation.z = data.startRotation;
				part.velocity = data.velocity;
				particles.insert(particles.begin(), part);
			}
		}
	}

	void ParticleSystemComponent::draw() {
		ST_PROFILING_FUNCTION();
		for (int i = 0; i < particles.size(); i++) {
			Particle& p = particles[i];
			if (p.alive || !data.imortal) {
				Stulu::Renderer2D::drawQuad(Stulu::Math::createMat4(p.pos, glm::quat(glm::radians(p.rotation)), p.cSize), p.cColor);
				continue;
			}
		}
	}
	void ParticleSystemComponent::update(Stulu::Timestep ts) {
		ST_PROFILING_FUNCTION();
		for (int i = 0; i < particles.size(); i++) {
			Particle& p = particles[i];
			p.alive = p.lifeTime < p.dieTime;
			if (p.alive || !data.imortal) {
				p.lifeTime += ts;

				float life = p.lifeTime / p.dieTime;

				if (data.moving) {
					if (data.randomVelcity)
						p.pos += Stulu::Random::getVector3(data.minVelocity, data.maxVelocity) * data.speed * ts.getSeconds();
					else
						p.pos += data.velocity * data.speed * ts.getSeconds();
					p.pos.y += data.gravity * ts;
				}
				if (data.rotate) {
					if (data.randomRotationSpeed)
						p.rotation.z += Stulu::Random::getFloat(data.randomRotationRange.x, data.randomRotationRange.y) * ts;
					else
						p.rotation.z += data.rotationSpeed * ts;
				}

				p.cColor = glm::lerp(p.beginColor, p.endColor, life);
				p.cSize = glm::lerp(p.beginSize, p.endSize, life);
				continue;
			}

			particles.erase(particles.begin() + i);
		}
	}
	void ParticleSystemComponent::updateDraw(Stulu::Timestep ts) {
		for (int i = 0; i < particles.size(); i++) {
			Particle& p = particles[i];
			p.alive = p.lifeTime < p.dieTime;
			if (p.alive || !data.imortal) {
				p.lifeTime += ts;

				float life = p.lifeTime / p.dieTime;

				if (data.moving) {
					if (data.randomVelcity)
						p.pos += Stulu::Random::getVector3(data.minVelocity, data.maxVelocity) * data.speed * ts.getSeconds();
					else
						p.pos += data.velocity * data.speed * ts.getSeconds();
					p.pos.y += data.gravity * ts;
				}
				if (data.rotate) {
					if (data.randomRotationSpeed)
						p.rotation.z += Stulu::Random::getFloat(data.randomRotationRange.x, data.randomRotationRange.y) * ts;
					else
						p.rotation.z += data.rotationSpeed * ts;
				}

				p.cColor = glm::lerp(p.beginColor, p.endColor, life);
				p.cSize = glm::lerp(p.beginSize, p.endSize, life);

				Stulu::Renderer2D::drawQuad(Stulu::Math::createMat4(p.pos, glm::quat(glm::radians(p.rotation)), p.cSize), p.cColor);
				continue;
			}

			particles.erase(particles.begin() + i);
		}
	}
	void ParticleSystemComponent::clear() {
		particles.clear();
	}
}
