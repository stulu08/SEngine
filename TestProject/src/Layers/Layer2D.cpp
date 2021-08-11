#include "Layer2D.h"

#include "TestProject.h"
void Layer2D::onAttach() {
	Stulu::RenderCommand::setClearColor(glm::vec4(.15f));
	Stulu::RenderCommand::setWireFrame(true);
}
#define PROFILE_RESULT(name) Timer timer##__LINE__(name, [&](ProfileResult result) {profileResults.push_back(result); })
void Layer2D::onUpdate(Stulu::Timestep timestep) {

	PROFILE_RESULT("onUpdate");
	{
		PROFILE_RESULT("camera update");
		m_cameraController.onUpdate(timestep);
	}
	{
		PROFILE_RESULT("draw preperation");
		Stulu::RenderCommand::clear();
		Stulu::Renderer2D::beginScene(m_cameraController.getCamera());
	}
	{
		PROFILE_RESULT("draw");
		Stulu::Renderer2D::drawQuad(m_transform, m_color);
		Stulu::Renderer2D::endScene();
	}

}

void Layer2D::onEvent(Stulu::Event& e) {
	m_cameraController.onEvent(e);
}
#include <imgui/imgui.h>
void Layer2D::onImguiRender(Stulu::Timestep timestep) {
	PROFILE_RESULT("onImguiRender");
	Stulu::Transform::ImGuiTransformEdit(m_transform, "Quad", true, false);
	ImGui::ColorEdit4("Color", glm::value_ptr(m_color));
	ImGui::End();
	ImGui::Begin("Results");
	for (auto& i : profileResults) {
		ImGui::Text("%.3fms : %s", i.time, i.name);
	}
	profileResults.clear();
	ImGui::End();
}
