#include "DefaultLayer.h"

#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Platform/OpenGL/OpenGLShader.h>

void DefaultLayer::onAttach() {
	Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
	Stulu::Ref<Stulu::IndexBuffer> indexBuffer;
	uint32_t cubeIndicies[6 * 6]{
		//top
		0,1,2,
		2,3,0,
		//bottom
		4,5,6,
		6,7,4,
		//right
		8,9,10,
		10,11,8,
		//left
		12,13,14,
		14,15,12,
		//left
		16,17,18,
		18,19,16,
		//left
		20,21,22,
		22,23,20,
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
		 //back
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

	m_cameraController.getCamera().setPosition(glm::vec3(0.0f, 0.0f, 1.0f));

}
void DefaultLayer::onEvent(Stulu::Event& e) {
	m_cameraController.onEvent(e);
}
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor(1.0f);
void DefaultLayer::onUpdate(Stulu::Timestep timestep) {
	//update
	m_cameraController.onUpdate(timestep);

	//render
	Stulu::RenderCommand::setClearColor(m_clearColor);
	Stulu::RenderCommand::clear();
	
	Stulu::Renderer::beginScene(m_cameraController.getCamera());
	{
		glm::mat4 transform = m_cubeTransform.getMat4();
		std::dynamic_pointer_cast<Stulu::OpenGLShader>(m_shaderLib.get("basiclight"))->uploadFloat3Uniform("u_color", glm::vec4(1.0f));
		std::dynamic_pointer_cast<Stulu::OpenGLShader>(m_shaderLib.get("basiclight"))->uploadFloat3Uniform("u_lightColor", lightColor);
		std::dynamic_pointer_cast<Stulu::OpenGLShader>(m_shaderLib.get("basiclight"))->uploadFloat3Uniform("u_lightPos", lightPos);
		Stulu::Renderer::submit(m_cubeVertexArray, m_shaderLib.get("basiclight"), transform);
	}
	Stulu::Renderer::endScene();
}
//imgui
#include <imgui/imgui.cpp>
void DefaultLayer::onImguiRender(Stulu::Timestep timestep) {
	Stulu::Transform::ImGuiTransformEdit(m_cubeTransform, "Cube");
	drawRendererInfos();
	drawCameraInfos();
	drawApplicationInfos(timestep);
}
void DefaultLayer::drawRendererInfos() {
	ImGui::Begin(std::string(Stulu::Application::get().graphicDriverConstants.vendor + " Renderer").c_str(), NULL, ImGuiWindowFlags_NoResize);
	ImGui::Text(Stulu::Application::get().graphicDriverConstants.renderer.c_str());
	ImGui::Text("Version %s", Stulu::Application::get().graphicDriverConstants.version.c_str());
	ImGui::Text("GLSL version %s", Stulu::Application::get().graphicDriverConstants.other.c_str());
	ImGui::End();

}
void DefaultLayer::drawCameraInfos() {
	ImGui::Begin("Enviroment", NULL, ImGuiWindowFlags_NoResize);
	ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor));
	ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));
	ImGui::DragFloat3("Light Pos", glm::value_ptr(lightPos));
		
	ImGui::End();
}
void DefaultLayer::drawApplicationInfos(Stulu::Timestep timestep) {
	ImGui::Begin("Application");
	ImGui::Text("Deltatime: %.3fs(%dms)", (float)timestep, (int)timestep.getMilliseconds());
	ImGui::Text("FPS: %.2f", 1.0f / timestep);
	ImGui::End();
}