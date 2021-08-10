#include "DemoGameLayer.h"

#include <imgui/imgui.h>
#include <glm/glm/gtc/type_ptr.hpp>

void DemoGameLayer::onAttach() {
}

void DemoGameLayer::onDetach() {

}

void DemoGameLayer::onUpdate(Timestep timestep) {
	player.update(timestep);
	m_map.update(timestep, player.getCam().getPosition().x);

	RenderCommand::clear();
	Renderer2D::beginScene(player.getCam());
	player.draw(); 
	m_map.draw();
	Renderer2D::drawQuad(Transform(glm::vec3(player.getCam().getPosition().x , 1.5f, 1.0f), glm::vec3(.0f), glm::vec3(5.0f, 1.0f, 1.0f)), glm::vec4(.1f, .8f, .9f, 1.0f));
	Renderer2D::drawQuad(Transform(glm::vec3(player.getCam().getPosition().x , -1.5f, 1.0f), glm::vec3(.0f), glm::vec3(5.0f, 1.0f, 1.0f)), glm::vec4(.1f, .8f, .9f, 1.0f));
	Renderer2D::endScene();
}

void DemoGameLayer::onImguiRender(Timestep timestep) {
	ImGui::Begin("Debug");
	ImGui::Text("Sore %.2f", player.getCam().getPosition().x);
	ImGui::End();
	m_map.imgui();
}

void DemoGameLayer::onEvent(Event& e) {
}
