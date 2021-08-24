#include "EditorLayer.h"
#include <imgui/imgui.h>
#include <EditorApp.h>

EditorLayer::EditorLayer()
	: Layer("EditorLayer"), m_cameraController(1280.0f / 720.0f, m_fov, .001f, 100.0f), m_particleSystem(&m_particleSystemData) {
;
	m_citySkyBox = Stulu::CubeMap::create({
		"assets/SkyBox/city/right.jpg",
		"assets/SkyBox/city/left.jpg",
		"assets/SkyBox/city/top.jpg",
		"assets/SkyBox/city/bottom.jpg",
		"assets/SkyBox/city/front.jpg",
		"assets/SkyBox/city/back.jpg"
		});
	m_skybox = Skybox(m_citySkyBox);
	m_galaxySkyBox = Stulu::CubeMap::create({
		"assets/SkyBox/5kgalaxy/right.jpg",
		"assets/SkyBox/5kgalaxy/left.jpg",
		"assets/SkyBox/5kgalaxy/top.jpg",
		"assets/SkyBox/5kgalaxy/bottom.jpg",
		"assets/SkyBox/5kgalaxy/front.jpg",
		"assets/SkyBox/5kgalaxy/back.jpg"
		});

	m_defaultSkybox = Stulu::CubeMap::create({
	"Stulu/assets/SkyBox/default/right.jpg",
	"Stulu/assets/SkyBox/default/left.jpg",
	"Stulu/assets/SkyBox/default/top.jpg",
	"Stulu/assets/SkyBox/default/bottom.jpg",
	"Stulu/assets/SkyBox/default/front.jpg",
	"Stulu/assets/SkyBox/default/back.jpg"
		});;

	m_model = Stulu::Model("assets/sphere.obj");
	m_shaderLib.load("Stulu/assets/Shaders/Reflective.glsl");
	Stulu::RenderCommand::setClearColor(glm::vec4(0.15f));
}

void EditorLayer::onAttach() {

	m_particleSystem.setMaxParticleCount(2000);
}

void EditorLayer::onUpdate(Stulu::Timestep timestep) {
	m_cameraController.onUpdate(timestep);

	m_particleSystem.emit(glm::vec3(.0f,.0f,.0f));

	Stulu::RenderCommand::clear();

	Stulu::Renderer::beginScene(m_cameraController.getCamera());

	m_skybox.draw(m_cameraController.getCamera());

	m_model.submitToRenderer(m_shaderLib.get("Reflective"), m_transform);
	m_shaderLib.get("Reflective")->setFloat("u_metallic", m_metalness);

	Stulu::Renderer2D::beginScene(m_cameraController.getCamera());

	m_particleSystem.draw(timestep);

	Stulu::Renderer2D::endScene();

	Stulu::Renderer::endScene();
}

void EditorLayer::onImguiRender(Stulu::Timestep timestep) {
	//ImGui::DockSpaceOverViewport();

	ImGui::Begin("Inspector");
	Stulu::imGui::Transform(m_transform,"Sphere",false,false,false);
	ImGui::Text("Material");
	Stulu::imGui::SliderFloat("Metalness", &m_metalness);
	ImGui::Text("Skybox");
	if(ImGui::Button("City")) {

		m_skybox.setCubeMap(m_citySkyBox);
	}
	ImGui::SameLine();
	if (ImGui::Button("Default")) {

		m_skybox.setCubeMap(m_defaultSkybox);
	}
	ImGui::SameLine();
	if (ImGui::Button("Galaxy")) {

		m_skybox.setCubeMap(m_galaxySkyBox);
	}
	if (Stulu::imGui::SliderFloat("Fov",&m_fov,20.0f,140.0f)) {
		m_cameraController.getCamera().setProjection(m_fov, m_cameraController.getAspectRatio(),.001f,100.0f);
	}
	ImGui::End();

	ImGui::Begin("Particles");
	ImGui::Text("FPS: %.0f", 1.0f / timestep);
	ImGui::Text("%d/%d Particels", m_particleSystem.getActiveParticleCount(), m_particleSystem.getMaxParticleCount());
	if (ImGui::Button("Clear"))
		m_particleSystem.clear();
	ParticleSystemData::imGuiDraw("Particles", m_particleSystemData, false);
}

void EditorLayer::onEvent(Stulu::Event& e) {
	m_cameraController.onEvent(e);
}
