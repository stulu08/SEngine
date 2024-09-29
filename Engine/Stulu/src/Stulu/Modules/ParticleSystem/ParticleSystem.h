#pragma once
#include <Stulu.h>

namespace Stulu {
	struct ParticleSystemData {
		ST_ENUM(EmitStart) {
			SceneStart, SceneUpdate, Manual
		};

		EmitStart emitStart = EmitStart::SceneStart;
		uint32_t emitCount = 20;

		bool moving = true;
		float speed = 1.0f;
		float gravity = -.0f;

		bool randomVelcity = true;
		glm::vec3 velocity = glm::vec3(.0f, 1.0f, .0f);
		glm::vec3 minVelocity = glm::vec3(-0.2f, .3f, -.2f), maxVelocity = glm::vec3(0.2f, 1.0f, 0.2f);

		bool imortal = true;
		float lifeTime = 2.0f;
		bool randomLifeTime = false;//random
		float randomLifeTimeMin = .0f, randomLifeTimeMax = .0f;

		bool rotate = true;
		float rotationSpeed = 50.0f;
		glm::vec2 randomRotationRange = glm::vec2(-30.0f, 30.0f);

		float startRotation = .0f;
		bool randomStartRotation = true;//random
		bool randomRotationSpeed = true;//random

		glm::vec3 beginSize = glm::vec3(.02f), endSize = glm::vec3(.1f);

		bool randomSize = true;//random
		glm::vec3 beginSizeMin = glm::vec3(.01f), endSizeMin = glm::vec3(.1f);
		glm::vec3 beginSizeMax = glm::vec3(.05f), endSizeMax = glm::vec3(.5f);

		glm::vec4 beginColor = glm::vec4(1.0f, 0.647, 0.0f, 1.0f), endColor = glm::vec4(1.0f, 0.647, 0.0f, .0f);
	};
	struct Particle {
		glm::vec3 pos;
		glm::vec3 velocity;
		glm::vec3 endSize, beginSize, cSize;
		glm::vec4 beginColor, endColor, cColor;
		glm::vec3 rotation;
		float lifeTime = .0f;
		float dieTime = .0f;
		bool alive = true;
	};
	class STULU_API ParticleSystemComponent : public Component {
	public:
		ParticleSystemComponent() = default;
		ParticleSystemComponent(const ParticleSystemComponent&) = default;
		ParticleSystemComponent(const ParticleSystemData& data, int maxPart = 1000)
			:data(data) {
			updateMaxParticleCount(maxPart);
		}

		void emit(const glm::vec3& pos, const glm::vec3 rotation, uint32_t count = 1);
		void draw();
		void update(Stulu::Timestep ts);
		void updateDraw(Stulu::Timestep ts);

		void clear();

		void updateMaxParticleCount(int maxParticle) { 
			maxParticle = maxParticle;
		}
		int getMaxParticleCount() {
			return maxParticle;
		}
		const size_t getActiveParticleCount() const { return (size_t)particles.size(); }

		ParticleSystemData& getData() { return data; }
		const ParticleSystemData& getData() const { return data; }

		virtual void destroy() override {
			clear();
		}
		virtual void onComponentAdded(Scene* scene) override {
			clear();
		}

		int maxParticle = 1000;
		ParticleSystemData data;
		std::vector<Particle> particles;
	};
}
