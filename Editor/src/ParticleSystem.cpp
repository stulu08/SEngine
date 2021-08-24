#include "ParticleSystem.h"

void ParticleSystem::emit(glm::vec3& pos, size_t count) {
	for (size_t i = 0; i < count; i++) {
		if (m_maxParticle > m_particles.size()) {
			Particle* part = new Particle();
			if (data->randomLifeTime)
				part->dieTime = Stulu::Random::getFloat(data->randomLifeTimeMin, data->randomLifeTimeMax);
			else
				part->dieTime = data->lifeTime;
			if (data->randomSize) {
				part->beginSize = Stulu::Random::getVector3(data->beginSizeMin, data->beginSizeMax);
				part->endSize = Stulu::Random::getVector3(data->endSizeMin, data->endSizeMax);
			}
			else {
				part->beginSize = data->beginSize;
				part->endSize = data->endSize;
			}
			if (data->randomVelcity)
				part->velocity = Stulu::Random::getVector3(data->minVelocity, data->maxVelocity);

			part->beginColor = data->beginColor;
			part->endColor = data->endColor;

			part->lifeTime = .0f;

			part->pos = pos;
			if (data->randomStartRotation)
				part->rotation = Stulu::Random::getFloat(.0f, 360.0f);
			else
				part->rotation = data->startRotation;
			part->velocity = data->velocity;
			m_particles.insert(m_particles.begin(), part);
		}
	}
}

void ParticleSystem::draw(Stulu::Timestep ts) {
	for (int i = 0; i < m_particles.size(); i++) {
		Particle* p = m_particles[i];
		p->alive = p->lifeTime < p->dieTime;
		if (p->alive || !data->dying) {
			p->lifeTime += ts;

			float life = p->lifeTime / p->dieTime;

			if (data->moving) {
				if(data->randomVelcity)
					p->pos += Stulu::Random::getVector3(data->minVelocity, data->maxVelocity) * data->speed * ts.getSeconds();
				else
					p->pos += data->velocity * data->speed * ts.getSeconds();
				p->pos.y += data->gravity * ts;
			}
			if (data->rotate) {
				if (data->randomRotationSpeed)
					p->rotation += Stulu::Random::getFloat(data->randomRotationRange.x, data->randomRotationRange.y) * ts;
				else
					p->rotation += data->rotationSpeed * ts;
			}

			p->cColor = glm::lerp(p->beginColor, p->endColor, life);
			p->cSize = glm::lerp(p->beginSize, p->endSize, life);

			Stulu::Renderer2D::drawQuad(p->pos, p->cSize, p->rotation, p->cColor);
			continue;
		}

		m_particles.erase(m_particles.begin() + i);
	}
	
}
void ParticleSystem::clear() {
	m_particles.clear();
}
#include <imgui/imgui.h>
void ParticleSystemData::imGuiDraw(const char* title, ParticleSystemData& data, bool begin, bool end) {
	if(begin)
		ImGui::Begin(title);
	ImGui::Checkbox("Moving", &data.moving);
	if (data.moving) {
		Stulu::imGui::Float("Speed", data.speed);
		Stulu::imGui::Float("Gravity", data.gravity);
		ImGui::Checkbox("Random Velocity", &data.randomVelcity);
		if (data.randomVelcity) {
			Stulu::imGui::vec3("Min Velocity", data.minVelocity);
			Stulu::imGui::vec3("Max Velocity", data.maxVelocity);
		}
		else
			Stulu::imGui::vec3("Velocity", data.velocity);
	}

	ImGui::Checkbox("Dying", &data.dying);
	if (data.dying) {
		ImGui::Checkbox("Random Lifetime", &data.randomLifeTime);
		if (data.randomLifeTime) {
			Stulu::imGui::Float("Min Lifetime", data.randomLifeTimeMin);
			Stulu::imGui::Float("Max Lifetime", data.randomLifeTimeMax);
		}
		else
			Stulu::imGui::Float("Lifetime", data.lifeTime);
	}
	ImGui::Checkbox("Rotate", &data.rotate);
	if (data.rotate) {

		ImGui::Checkbox("Random Start Rotation", &data.randomStartRotation);
		if (!data.randomStartRotation)
			Stulu::imGui::Float("Start Rotation", data.startRotation);

		ImGui::Checkbox("Random Rotation Speed", &data.randomRotationSpeed);
		if (data.randomRotationSpeed)
			Stulu::imGui::vec2("Random Rotation Speed Range", data.randomRotationRange);
		else
			Stulu::imGui::Float("Rotation Speed", data.rotationSpeed);
	}
	ImGui::Checkbox("Random Size", &data.randomSize);
	if (!data.randomSize) {
		Stulu::imGui::vec3("Begin Size", data.endSize);
		Stulu::imGui::vec3("End Size", data.beginSize);
	}
	else {
		Stulu::imGui::vec3("Begin Size min", data.beginSizeMin);
		Stulu::imGui::vec3("Begin Size max", data.beginSizeMax);
		Stulu::imGui::vec3("End Size min", data.endSizeMin);
		Stulu::imGui::vec3("End Size max", data.endSizeMax);
	}

	ImGui::ColorEdit4("Begin Color", glm::value_ptr(data.beginColor));
	ImGui::ColorEdit4("End Color", glm::value_ptr(data.endColor));

	if(end)
		ImGui::End();
}
