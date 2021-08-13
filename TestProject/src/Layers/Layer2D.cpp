#include "Layer2D.h"

#include "TestProject.h"
void Layer2D::onAttach() {
	Stulu::RenderCommand::setClearColor(glm::vec4(.15f));

	//m_texture = Stulu::Texture2D::create("assets/light.png");
}
void Layer2D::onUpdate(Stulu::Timestep timestep) {


	m_cameraController.onUpdate(timestep);

	Stulu::RenderCommand::clear();
	Stulu::Renderer2D::beginScene(m_cameraController.getCamera());

	//Stulu::Renderer2D::drawTexture2DQuad(m_texture,m_transform, m_color);
	Stulu::Renderer2D::drawQuad(glm::vec3(0.0f,.0f,-5.0f),glm::vec2(1.0f,1.0f), m_colorOne);
	Stulu::Renderer2D::drawQuad(glm::vec3(-1.0f,-1.0f,-3.0f),glm::vec2(1.0f,1.0f), m_colorTwo);
	Stulu::Renderer2D::endScene();

}

void Layer2D::onEvent(Stulu::Event& e) {
	m_cameraController.onEvent(e);
}
#include <imgui/imgui.h>
void Layer2D::onImguiRender(Stulu::Timestep timestep) {
	ST_PROFILING_FUNCTION();
	Stulu::imGui::Transform(m_transformOne, "Quad One", true, false);
	ImGui::ColorEdit4("Color One", glm::value_ptr(m_colorOne));
	Stulu::imGui::Transform(m_transformTwo, "Quad Two", true, false, false);
	ImGui::ColorEdit4("Color Two", glm::value_ptr(m_colorTwo));
	ImGui::End();
}
