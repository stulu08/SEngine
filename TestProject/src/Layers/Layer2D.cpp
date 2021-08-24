#include "Layer2D.h"

#include "TestProject.h"
void Layer2D::onAttach() {
	Stulu::RenderCommand::setClearColor(glm::vec4(.15f));
	particleSystem.setMaxParticleCount(20000);
	m_transformOne.position.x += 1;
	m_transformTwo.position.x -= 1;
	m_transformThree.position.y += 1;
	m_texture = Stulu::Texture2D::create("Stulu/assets/Textures/light.png");
}
void Layer2D::onUpdate(Stulu::Timestep timestep) {
	m_cameraController.onUpdate(timestep);

	Stulu::RenderCommand::clear();
	Stulu::Renderer2D::beginScene(m_cameraController.getCamera());

	Stulu::Renderer2D::drawQuad(m_transformOne, m_colorOne);
	Stulu::Renderer2D::drawQuad(m_transformTwo, m_colorTwo);
	Stulu::Renderer2D::drawTexturedQuad(m_transformThree, m_texture, m_colorThree);

	particleSystem.draw(timestep);

	if (Stulu::Input::isMouseDown(MOUSE_BUTTON_1) && m_particleMouseControl) {
		m_particlePos = Stulu::Math::screenToWorld(glm::vec2(Stulu::Input::getMouseX(), Stulu::Input::getMouseY()), m_cameraController.getCamera(), m_cameraController.getScreenSize());
		m_particlePos.z = .1f;
		particleSystem.emit(glm::vec3(m_particlePos), m_pSpawnCount);
	}else if(!m_particleMouseControl)
		particleSystem.emit(glm::vec3(m_particlePos), m_pSpawnCount);

	Stulu::Renderer2D::endScene();
}

void Layer2D::onEvent(Stulu::Event& e) {
	m_cameraController.onEvent(e);
}
#include <imgui/imgui.h>
void Layer2D::onImguiRender(Stulu::Timestep timestep) {
	ST_PROFILING_FUNCTION();
	ImGui::Begin("Objects");
	Stulu::imGui::Transform(m_transformOne, "Quad One", false, false, false);
	ImGui::ColorEdit4("Color One", glm::value_ptr(m_colorOne));
	Stulu::imGui::Transform(m_transformTwo, "Quad Two", false, false, false);
	ImGui::ColorEdit4("Color Two", glm::value_ptr(m_colorTwo));
	Stulu::imGui::Transform(m_transformThree, "Texture", false, false, false);
	Stulu::imGui::vec2("Tiling", m_texture->tiling);
	ImGui::ColorEdit4("Color", glm::value_ptr(m_colorThree));
	ImGui::End();

	ImGui::Begin("Particles");
	ImGui::Text("FPS: %.0f", 1.0f / timestep);
	ImGui::Text("%d/%d Particels", particleSystem.getActiveParticleCount(), particleSystem.getMaxParticleCount());
	ImGui::SliderInt("Spawn Count", &m_pSpawnCount, 0, 50);
	Stulu::imGui::vec3("Spawn Position", m_particlePos);
	ImGui::Checkbox("Mouse Control", &m_particleMouseControl);
	if (ImGui::Button("Clear"))
		particleSystem.clear();
	ParticleSystemData::imGuiDraw("Particles", particleSystemData,false);

}
