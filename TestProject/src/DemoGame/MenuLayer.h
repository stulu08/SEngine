#pragma once
#include <Stulu.h>
#include "DemoGameLayer.h"

class Menu : public Stulu::Layer {
public:
	Menu()
		: Layer("Menu"), m_camController(1280.0f / 720.0f){

	}
	virtual void onAttach() {
		Stulu::RenderCommand::setClearColor(glm::vec4(.01f));
		m_texture = Texture2D::create("assets/triangle.png");

	}
	virtual void onUpdate(Stulu::Timestep timestep) override {
		Stulu::RenderCommand::clear();
		Stulu::Renderer2D::beginScene(m_camController.getCamera());
		Stulu::Renderer2D::drawTexture2DQuad(m_texture, glm::vec2(.5f, .5f), glm::vec2(.5f, .5f), glm::vec3(.0f, .0f, -90.0f));
		Stulu::Renderer2D::endScene();
	}

private:
	OrthographicCameraController m_camController;
	Stulu::Ref<Texture2D> m_texture;
};