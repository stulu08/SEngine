#include "GameLayer.h"
#include <imgui/imgui.h>
#include <glm/glm/gtc/type_ptr.hpp>

void GameLayer::onAttach() {
	m_cameraController.getCamera().setPosition(glm::vec3(0.0f,0.0f,30.0f));
	m_texture = Stulu::Texture2D::create("assets/Logo/engine-logo.png");
}
void GameLayer::onDetach() {
	
}
void GameLayer::onUpdate(Stulu::Timestep timestep) {
	m_cameraController.onUpdate(timestep);

	float distance = glm::distance(m_cameraController.getCamera().getPosition(), quadTransform.getPos());

	//render
	Stulu::RenderCommand::setClearColor(m_clearColor);
	Stulu::RenderCommand::clear();
	Stulu::Renderer2D::beginScene(m_cameraController.getCamera());
	Stulu::Renderer2D::drawQuad(quadTransform, { .2f,.8f,.3f, 1.0f });
	Stulu::Renderer2D::drawTriangle(triangleTransform, { .2f,.3f,.8f, 1.0f });
	Stulu::Renderer2D::drawTexture2D(m_texture,textureTransform);
	Stulu::Renderer2D::endScene();
}
void GameLayer::onImguiRender(Stulu::Timestep timestep) {
	ImGui::Begin("Enviroment", NULL, ImGuiWindowFlags_NoResize);
	ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor));
	ImGui::End();
	Stulu::Transform::ImGuiTransformEdit(quadTransform, "Quad");
	Stulu::Transform::ImGuiTransformEdit(triangleTransform, "Triangle");
	Stulu::Transform::ImGuiTransformEdit(textureTransform, "Texture");
}
void GameLayer::onEvent(Stulu::Event& e) {
	m_cameraController.onEvent(e);
}