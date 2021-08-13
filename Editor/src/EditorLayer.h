#pragma once
#include <Stulu.h>

class EditorLayer : public Stulu::Layer{
public:
	EditorLayer()
		: Layer("EditorLayer"), m_cameraController(1280.0f / 720.0f, m_fov, .001f, 100.0f) {
		load(); 
	}

	void load();
	void onAttach() override;
	void onUpdate(Stulu::Timestep timestep) override;
	void onImguiRender(Stulu::Timestep timestep) override;
	void onEvent(Stulu::Event& e) override;
private:
	Stulu::Transform m_transform = Stulu::Transform(glm::vec3(0.0f,0.0f,-3.0f));
	float m_metalness = 1.0f;
	float m_fov = 85.0f;
	Stulu::Model m_model;

	Stulu::Ref<Stulu::CubeMap> m_oceanSkyBox;
	Stulu::Ref<Stulu::CubeMap> m_citySkyBox;
	Stulu::Ref<Stulu::CubeMap> m_galaxySkyBox;

	Stulu::PerspectiveCameraController m_cameraController;

	Stulu::ShaderLibary m_shaderLib;
};