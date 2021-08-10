#pragma once
#include <Stulu.h>

class GameLayer : public Stulu::Layer {
public:
	GameLayer()
		: Layer("Game"), m_cameraController(1280.0f / 720.0f,true, true, true){}

	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onUpdate(Stulu::Timestep timestep) override;
	virtual void onImguiRender(Stulu::Timestep timestep) override;
	virtual void onEvent(Stulu::Event& e) override;

private:
	Stulu::Ref<Stulu::Texture2D> m_texture;
	Stulu::OrthographicCameraController m_cameraController;

	glm::vec4 m_clearColor = glm::vec4(COLOR_GRAY_VEC4);
	Stulu::Transform quadTransform;
	Stulu::Transform triangleTransform;
	Stulu::Transform textureTransform;
};

