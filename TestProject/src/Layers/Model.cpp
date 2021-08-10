#include "Model.h"

#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

void ModelLayer::onAttach() {
	m_shaderLib.load("assets/Shaders/basiclight.glsl");
	m_lightIconTexture = Stulu::Texture2D::create("assets/light.png");
	m_cameraController.setTransform(Stulu::Transform(glm::vec3(-3.6f, 4.5f, 8.7f), glm::vec3(-23.0f, -64.0f, .0f)));
}
void ModelLayer::onEvent(Stulu::Event& e) {
	m_cameraController.onEvent(e);
}
void ModelLayer::onUpdate(Stulu::Timestep timestep) {
	//update
	m_cameraController.onUpdate(timestep);

	//render
	Stulu::RenderCommand::setClearColor(m_clearColor);
	Stulu::RenderCommand::clear();

	Stulu::Renderer::beginScene(m_cameraController.getCamera());
	m_shaderLib.get("basiclight")->bind();
	m_shaderLib.get("basiclight")->setFloat3("u_color", glm::vec4(1.0f));
	m_shaderLib.get("basiclight")->setFloat3("u_lightColor", lightColor);
	m_shaderLib.get("basiclight")->setFloat3("u_lightPos", lightPos);
	m_shaderLib.get("basiclight")->setFloat("u_lightStrength", lightStrength);
	Stulu::Renderer2D::beginScene(m_cameraController.getCamera());
	Stulu::Renderer2D::drawTexture2DQuad(m_lightIconTexture, lightPos, glm::vec2(1, 1), m_cameraController.getTransform().getRotation(), glm::vec4(lightColor, 1.0f));
	Stulu::Renderer2D::endScene();
	Stulu::Renderer::endScene();
}
//imgui
#include <imgui/imgui.h>
void ModelLayer::onImguiRender(Stulu::Timestep timestep) {
	Stulu::Transform::ImGuiTransformEdit(m_cameraController.getTransform(), "Camera");
	drawRendererInfos();
	drawCameraInfos();
	drawApplicationInfos(timestep);
}
void ModelLayer::drawRendererInfos() {
	ImGui::Begin(std::string(Stulu::Application::get().graphicDriverConstants.vendor + " Renderer").c_str(), NULL, ImGuiWindowFlags_NoResize);
	ImGui::Text(Stulu::Application::get().graphicDriverConstants.renderer.c_str());
	ImGui::Text("Version %s", Stulu::Application::get().graphicDriverConstants.version.c_str());
	ImGui::Text("GLSL version %s", Stulu::Application::get().graphicDriverConstants.other.c_str());
	ImGui::End();

}
void ModelLayer::drawCameraInfos() {
	ImGui::Begin("Enviroment", NULL, ImGuiWindowFlags_NoResize);
	ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor));
	ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));
	ImGui::DragFloat3("Light Pos", glm::value_ptr(lightPos));
	ImGui::DragFloat("Light Strength", &lightStrength, .01f, .0f, 1.0f);

	ImGui::End();
}
void ModelLayer::drawApplicationInfos(Stulu::Timestep timestep) {
	ImGui::Begin("Application");
	ImGui::Text("Deltatime: %.3fs(%dms)", (float)timestep, (int)timestep.getMilliseconds());
	ImGui::Text("FPS: %.2f", 1.0f / timestep);
	ImGui::End();
}