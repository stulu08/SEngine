#include "ModelLayer.h"

#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <TestProject.h>
void ModelLayer::onAttach() {
	m_shaderLib.load("assets/Shaders/pbr.glsl");
	m_shaderLib.load("assets/Shaders/Reflective.glsl");
	m_lightIconTexture = Stulu::Texture2D::create("assets/light.png");
	m_cameraController.setTransform(Stulu::Transform(glm::vec3(-3.6f, 4.5f, 8.7f), glm::vec3(-23.0f, -64.0f, .0f)));
	m_modelSphere = Stulu::Model("assets/girl.obj");
	m_modelCar = Stulu::Model("assets/mclaren.obj");

	std::vector<std::string> faces =
	{
		"assets/SkyBox/yokoham/right.jpg",
		"assets/SkyBox/yokoham/left.jpg",
		"assets/SkyBox/yokoham/top.jpg",
		"assets/SkyBox/yokoham/bottom.jpg",
		"assets/SkyBox/yokoham/front.jpg",
		"assets/SkyBox/yokoham/back.jpg"
	};
	/*std::vector<std::string> faces =
	{
		"assets/SkyBox/default/right.jpg",
		"assets/SkyBox/default/left.jpg",
		"assets/SkyBox/default/top.jpg",
		"assets/SkyBox/default/bottom.jpg",
		"assets/SkyBox/default/front.jpg",
		"assets/SkyBox/default/back.jpg"
	};*/
	Stulu::Renderer::setSkyBox(Stulu::CubeMap::create(faces));

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

	Stulu::Renderer::beginScene(m_cameraController.getCamera(), true);
	m_shaderLib.get("pbr")->bind();


	m_shaderLib.get("pbr")->setFloat3("u_lightPosition", m_sphereTransform.getPos());
	m_shaderLib.get("pbr")->setFloat3("u_lightColor", lightData.lightColor);
	m_shaderLib.get("pbr")->setFloat("u_lightStrength", lightData.lightStrength);
	m_shaderLib.get("pbr")->setFloat3("u_camPos", m_cameraController.getTransform().getPos());
	m_modelCar.submitToRenderer(m_shaderLib.get("pbr"), m_carTransform);


	m_shaderLib.get("Reflective")->bind();
	m_shaderLib.get("Reflective")->setFloat("u_metallic", 1.0f);
	m_shaderLib.get("Reflective")->setFloat3("u_camPos", m_cameraController.getTransform().getPos());
	m_modelSphere.submitToRenderer(m_shaderLib.get("Reflective"), m_sphereTransform);

	Stulu::Renderer::endScene();
}
//imgui
#include <imgui/imgui.h>
void ModelLayer::onImguiRender(Stulu::Timestep timestep) {
	Stulu::Transform::ImGuiTransformEdit(m_cameraController.getTransform(), "Camera");

	Stulu::Transform::ImGuiTransformEdit(m_carTransform, "Weapon");
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
	Stulu::Transform::ImGuiTransformEdit(m_sphereTransform, "Light", false, false, false);
	ImGui::ColorEdit3("Light Color", glm::value_ptr(lightData.lightColor));
	ImGui::DragFloat("Light Strength", &lightData.lightStrength);
	ImGui::End();
}
void ModelLayer::drawApplicationInfos(Stulu::Timestep timestep) {
	ImGui::Begin("Application");
	ImGui::Text("Deltatime: %.3fs(%dms)", (float)timestep, (int)timestep.getMilliseconds());
	ImGui::Text("FPS: %.2f", 1.0f / timestep);
	ImGui::Text("Vertices: %d", m_modelCar.getVerticesCount() + m_modelSphere.getVerticesCount());
	ImGui::Text("Indices: %d", m_modelCar.getIndicesCount() + m_modelSphere.getIndicesCount());
	ImGui::Text("Triangles: %d", m_modelCar.getTriangleCount() + m_modelSphere.getTriangleCount());
	ImGui::End();
}