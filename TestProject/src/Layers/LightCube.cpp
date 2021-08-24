#include "LightCube.h"

#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

void LightCube::onAttach() {
	Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
	Stulu::Ref<Stulu::IndexBuffer> indexBuffer;
	uint32_t cubeIndicies[6 * 6]{
		//top
		2,1,0,
		0,3,2,
		//bottom
		4,5,6,
		6,7,4,
		//right
		10,9,8,
		8,11,10,
		//left
		12,13,14,
		14,15,12,
		//front
		16,17,18,
		18,19,16,
		//back
		22,21,20,
		20,23,22
	};
	float cubeVertices[6 * 6 * 4] = {
		//top
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 //bottom 
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 //right 
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		//left
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		 //front
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 //back
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	};
	m_cubeVertexArray = Stulu::VertexArray::create();
	vertexBuffer = Stulu::VertexBuffer::create(sizeof(cubeVertices), cubeVertices);
	Stulu::BufferLayout cubeLayout = {
		{ Stulu::ShaderDataType::Float3, "a_pos" },
		{ Stulu::ShaderDataType::Float3, "a_normal" },
	};
	vertexBuffer->setLayout(cubeLayout);
	m_cubeVertexArray->addVertexBuffer(vertexBuffer);
	indexBuffer = Stulu::IndexBuffer::create(sizeof(cubeIndicies) / sizeof(uint32_t), cubeIndicies);
	m_cubeVertexArray->setIndexBuffer(indexBuffer);

	m_shaderLib.load("assets/Shaders/basiclight.glsl");
	m_shaderLib.load("assets/Shaders/color.glsl");
	

	for (int i = 0 ,x = 0; x != 10; x++) {
		for (int y = 0; y != 10; y++) {
			map[i] = Cube(Stulu::Transform(glm::vec3(x,sin(x) + cos(y),y),glm::vec3(.0f),glm::vec3(1.0f)));
			i++;
		}
	}
	m_cameraController.setTransform(Stulu::Transform(glm::vec3(-3.6f, 4.5f, 8.7f), glm::vec3(-23.0f, -64.0f, .0f),glm::vec3(1.0f)));
}
void LightCube::onEvent(Stulu::Event& e) {
	m_cameraController.onEvent(e);
}
void LightCube::onUpdate(Stulu::Timestep timestep) {
	//update
	m_cameraController.onUpdate(timestep);

	//render
	Stulu::RenderCommand::clear();
	
	Stulu::Renderer::beginScene(m_cameraController.getCamera());
	m_shaderLib.get("basiclight")->bind();
	m_shaderLib.get("basiclight")->setFloat3("u_color", glm::vec4(1.0f));
	m_shaderLib.get("basiclight")->setFloat3("u_lightColor", lightColor);
	m_shaderLib.get("basiclight")->setFloat3("u_lightPos", lightPos);
	m_shaderLib.get("basiclight")->setFloat("u_lightStrength", lightStrength);
	for (int i = 0, x = 0; x != 10; x++) {
		for (int y = 0; y != 10; y++) {
			Stulu::Renderer::submit(m_cubeVertexArray, m_shaderLib.get("basiclight"), map[i].transform);
			i++;
		}
	}
	Stulu::Renderer::endScene();
}
//imgui
#include <imgui/imgui.h>
void LightCube::onImguiRender(Stulu::Timestep timestep) {
	Stulu::imGui::Transform(m_cameraController.getTransform(), "Camera");
	drawRendererInfos();
	drawCameraInfos();
	drawApplicationInfos(timestep);
}
void LightCube::drawRendererInfos() {
	ImGui::Begin(std::string(Stulu::Application::get().graphicDriverConstants.vendor + " Renderer").c_str(), NULL, ImGuiWindowFlags_NoResize);
	ImGui::Text(Stulu::Application::get().graphicDriverConstants.renderer.c_str());
	ImGui::Text("Version %s", Stulu::Application::get().graphicDriverConstants.version.c_str());
	ImGui::Text("GLSL version %s", Stulu::Application::get().graphicDriverConstants.other.c_str());
	ImGui::End();

}
void LightCube::drawCameraInfos() {
	ImGui::Begin("Enviroment", NULL, ImGuiWindowFlags_NoResize);
	ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor));
	ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));
	ImGui::DragFloat3("Light Pos", glm::value_ptr(lightPos));
	ImGui::DragFloat("Light Strength", &lightStrength,.01f,.0f,1.0f);
		
	ImGui::End();
}
void LightCube::drawApplicationInfos(Stulu::Timestep timestep) {
	ImGui::Begin("Application");
	ImGui::Text("Deltatime: %.3fs(%dms)", (float)timestep, (int)timestep.getMilliseconds());
	ImGui::Text("FPS: %.2f", 1.0f / timestep);
	ImGui::End();
}