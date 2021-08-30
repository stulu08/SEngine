#pragma once
#include <Stulu.h>

struct ParticleSystemData {
	bool moving = true;
	float speed = 1.0f;
	float gravity = -.0f;
	bool randomVelcity = true;
	glm::vec3 velocity = glm::vec3(.0f, 1.0f, .0f);
	glm::vec3 minVelocity = glm::vec3(-0.2f, .3f, -.2f), maxVelocity = glm::vec3(0.2f, 1.0f, 0.2f);

	bool dying = true;
	float lifeTime = 2.0f;
	bool randomLifeTime = false;//random
	float randomLifeTimeMin = .0f, randomLifeTimeMax = .0f;

	bool rotate = true;
	float startRotation = .0f;
	bool randomStartRotation = true;//random
	bool randomRotationSpeed = true;//random
	glm::vec2 randomRotationRange = glm::vec2(-30.0f, 30.0f);
	float rotationSpeed = 50.0f;


	glm::vec3 beginSize = glm::vec3(.02f), endSize = glm::vec3(.1f);
	bool randomSize = true;//random
	glm::vec3 beginSizeMin = glm::vec3(.01f), endSizeMin = glm::vec3(.1f);
	glm::vec3 beginSizeMax = glm::vec3(.05f), endSizeMax = glm::vec3(.5f);

	glm::vec4 beginColor = glm::vec4(1.0f, 0.647, 0.0f, 1.0f), endColor = glm::vec4(1.0f, 0.647, 0.0f, .0f);

	static void imGuiDraw(const char* title, ParticleSystemData& data, bool begin = true, bool end = true);
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
class ParticleSystem {
public:
	ParticleSystem(ParticleSystemData* data)
		:data(data) {}

	void emit(glm::vec3& pos, size_t count = 1);
	void draw(Stulu::Timestep ts, glm::vec3& rotation);

	void clear();

	void setMaxParticleCount(int maxParticle) { m_maxParticle = maxParticle; }
	int& getMaxParticleCount() { return m_maxParticle; }
	const size_t getActiveParticleCount() const { return (size_t)m_particles.size(); }

	ParticleSystemData* data;

private:
	int m_maxParticle = 1000;
	std::vector<Particle*> m_particles;
};