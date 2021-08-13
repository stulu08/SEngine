#include "EditorLayer.h"
#include <imgui/imgui.h>
#include <EditorApp.h>

void EditorLayer::load() {
	std::vector<std::string> DefaultSkyboxfaces =
	{
		"Stulu/assets/SkyBox/default/right.jpg",
		"Stulu/assets/SkyBox/default/left.jpg",
		"Stulu/assets/SkyBox/default/top.jpg",
		"Stulu/assets/SkyBox/default/bottom.jpg",
		"Stulu/assets/SkyBox/default/front.jpg",
		"Stulu/assets/SkyBox/default/back.jpg"
	};
	std::vector<std::string> CitySkyboxfaces =
	{
		"assets/SkyBox/city/right.jpg",
		"assets/SkyBox/city/left.jpg",
		"assets/SkyBox/city/top.jpg",
		"assets/SkyBox/city/bottom.jpg",
		"assets/SkyBox/city/front.jpg",
		"assets/SkyBox/city/back.jpg"
	};
	std::vector<std::string> GalaxySkyboxfaces =
	{
		"assets/SkyBox/5kgalaxy/right.jpg",
		"assets/SkyBox/5kgalaxy/left.jpg",
		"assets/SkyBox/5kgalaxy/top.jpg",
		"assets/SkyBox/5kgalaxy/bottom.jpg",
		"assets/SkyBox/5kgalaxy/front.jpg",
		"assets/SkyBox/5kgalaxy/back.jpg"
	};
	ST_INFO("Loading...");
	m_model = Stulu::Model("assets/sphere.obj");
	ST_INFO("Loaded sphere model");
	m_oceanSkyBox = Stulu::CubeMap::create(DefaultSkyboxfaces);
	ST_INFO("Loaded default-skybox");
	m_citySkyBox = Stulu::CubeMap::create(CitySkyboxfaces);
	ST_INFO("Loaded city-skybox");
	m_galaxySkyBox = Stulu::CubeMap::create(GalaxySkyboxfaces);
	ST_INFO("Loaded galaxy-skybox");
	Stulu::Renderer::setSkyBox(m_citySkyBox);
	m_shaderLib.load("Stulu/assets/Shaders/Reflective.glsl");
	Stulu::RenderCommand::setClearColor(glm::vec4(0.15f));
	ST_INFO("Done loading");
}

void EditorLayer::onAttach() {


}

void EditorLayer::onUpdate(Stulu::Timestep timestep) {
	m_cameraController.onUpdate(timestep);

	Stulu::RenderCommand::clear();

	Stulu::Renderer::beginScene(m_cameraController.getCamera(),true);
	m_model.submitToRenderer(m_shaderLib.get("Reflective"), m_transform);
	m_shaderLib.get("Reflective")->setFloat("u_metallic", m_metalness);
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

		Stulu::Renderer::setSkyBox(m_citySkyBox);
	}
	ImGui::SameLine();
	if (ImGui::Button("Ocean")) {

		Stulu::Renderer::setSkyBox(m_oceanSkyBox);
	}
	ImGui::SameLine();
	if (ImGui::Button("Galaxy")) {

		Stulu::Renderer::setSkyBox(m_galaxySkyBox);
	}
	if (Stulu::imGui::SliderFloat("Fov",&m_fov,20.0f,140.0f)) {
		m_cameraController.getCamera().setProjection(m_fov, m_cameraController.getAspectRatio(),.001f,100.0f);
	}
	ImGui::End();
}

void EditorLayer::onEvent(Stulu::Event& e) {
	m_cameraController.onEvent(e);
}
