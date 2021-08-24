#pragma once
#include "Skybox.h"
#include "ParticleSystem.h"

class EditorLayer : public Stulu::Layer{
public:
	EditorLayer();

	void onAttach() override;
	void onUpdate(Stulu::Timestep timestep) override;
	void onImguiRender(Stulu::Timestep timestep) override;
	void onEvent(Stulu::Event& e) override;
private:
	Stulu::Transform m_transform = Stulu::Transform(glm::vec3(0.0f,0.0f,-3.0f),glm::vec3(.0f),glm::vec3(1.0f));
	float m_metalness = 1.0f;
	float m_fov = 85.0f;
	Stulu::Model m_model;

	Stulu::PerspectiveCameraController m_cameraController;
	ParticleSystemData m_particleSystemData;
	ParticleSystem m_particleSystem;



	Stulu::Ref<Stulu::CubeMap> m_citySkyBox;
	Stulu::Ref<Stulu::CubeMap> m_defaultSkybox;
	Stulu::Ref<Stulu::CubeMap> m_galaxySkyBox;

	Skybox m_skybox;

	Stulu::ShaderLibary m_shaderLib;
};