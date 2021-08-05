#include "DefaultLayer.h"

#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Platform/OpenGL/OpenGLShader.h>

void DefaultLayer::onAttach() {
	Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
	Stulu::Ref<Stulu::IndexBuffer> indexBuffer;
	uint32_t squareIndicies[6]{
		0,1,2,
		2,3,0
	};
	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, .0f, 0.0f, 0.0f,
		 0.5f, -0.5f, .0f, 1.0f, 0.0f,
		 0.5f,  0.5f, .0f, 1.0f, 1.0f,
		-0.5f,  0.5f, .0f, 0.0f, 1.0f
	};
	m_squareVertexArray = Stulu::VertexArray::create();
	vertexBuffer = Stulu::VertexBuffer::create(sizeof(squareVertices), squareVertices);
	Stulu::BufferLayout squareLayout = {
		{ Stulu::ShaderDataType::Float3, "a_pos" },
		{ Stulu::ShaderDataType::Float2, "a_texCoord" },
	};
	vertexBuffer->setLayout(squareLayout);
	m_squareVertexArray->addVertexBuffer(vertexBuffer);
	indexBuffer= Stulu::IndexBuffer::create(sizeof(squareIndicies) / sizeof(uint32_t), squareIndicies);
	m_squareVertexArray->setIndexBuffer(indexBuffer);

	m_texture = Stulu::Texture2D::create("assets/Logo/engine-logo.png");
	m_shaderLib.load("assets/Shaders/color.glsl");
	m_shaderLib.load("assets/Shaders/texture.sshader");

	//std::dynamic_pointer_cast<Stulu::OpenGLShader>(m_textureShader)->bind();

}
void DefaultLayer::onEvent(Stulu::Event& e) {
	m_cameraController.onEvent(e);
	if (e.getEventType() == Stulu::EventType::WindowResize) {
		Stulu::WindowResizeEvent& resizeEvent = ((Stulu::WindowResizeEvent&)e);
		resizeEvent.getWidth();
		12 + 4;
	}
}
void DefaultLayer::onUpdate(Stulu::Timestep timestep) {
	//update
	m_cameraController.onUpdate(timestep);

	//render
	Stulu::RenderCommand::setClearColor(m_clearColor);
	Stulu::RenderCommand::clear();
	
	Stulu::Renderer::beginScene(m_cameraController.getCamera());
	{
		if(!false){
			glm::mat4 gridScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
			for (int x = 0; x < 10; x++) {
				for (int y = 0; y < 10; y++) {
					glm::vec3 gridPos = glm::vec3(x * 0.11f, y * 0.11f, 0.0f) + m_gridPos;
					glm::mat4 gridTransform = glm::translate(glm::mat4(1.0f), gridPos) * gridScale;
					if (y % 2 == 0 ? x % 2 == 0 : x % 2 != 0)
						std::dynamic_pointer_cast<Stulu::OpenGLShader>(m_shaderLib.get("color"))->uploadFloat4Uniform("u_color", m_colorOne);
					else
						std::dynamic_pointer_cast<Stulu::OpenGLShader>(m_shaderLib.get("color"))->uploadFloat4Uniform("u_color", m_colorTwo);
					Stulu::Renderer::submit(m_squareVertexArray, m_shaderLib.get("color"), gridTransform);
				}

			}
		}
		{
			glm::mat4 squaretransform = glm::translate(glm::mat4(1.0f), m_squarePos) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
			m_texture->bind();
			Stulu::Renderer::submit(m_squareVertexArray, m_shaderLib.get("texture"), squaretransform);
		}
	}
	Stulu::Renderer::endScene();

}
//imgui
#include <imgui/imgui.cpp>
void DefaultLayer::onImguiRender(Stulu::Timestep timestep) {
	drawRendererInfos();
	drawCameraInfos();
	drawGridMoveWindow();
	drawSquareMoveWindow();
	drawApplicationInfos(timestep);
}
void DefaultLayer::drawRendererInfos() {
	ImGui::Begin(std::string(app->graphicDriverConstants.vendor + " Renderer").c_str(), NULL, ImGuiWindowFlags_NoResize);
	ImGui::Text(app->graphicDriverConstants.renderer.c_str());
	ImGui::Text("Version %s", app->graphicDriverConstants.version.c_str());
	ImGui::Text("GLSL version %s", app->graphicDriverConstants.other.c_str());
	ImGui::End();

}
void DefaultLayer::drawCameraInfos() {
	ImGui::Begin("Enviroment", NULL, ImGuiWindowFlags_NoResize);
	ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor));
	if (ImGui::Button("Reset Color")) {
		m_clearColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
	}
		
	ImGui::End();
}
void DefaultLayer::drawApplicationInfos(Stulu::Timestep timestep) {
	ImGui::Begin("Application");
	ImGui::Text("Deltatime: %.3fs(%dms)", (float)timestep, (int)timestep.getMilliseconds());
	ImGui::Text("FPS: %.2f", 1.0f / timestep);
	ImGui::End();
}
void DefaultLayer::drawSquareMoveWindow() {
	ImGui::Begin("Square Transform", NULL, ImGuiWindowFlags_NoResize);
	ImGui::DragFloat3("Position", glm::value_ptr(m_squarePos), .1f);
	if (ImGui::Button("Reset Position")) {
		m_squarePos = glm::vec3(.0f, .0f, .0f);
	}
	ImGui::End();
}
void DefaultLayer::drawGridMoveWindow() {
	ImGui::Begin("Grid Transform", NULL, ImGuiWindowFlags_NoResize);
	ImGui::DragFloat3("Position", glm::value_ptr(m_gridPos), .1f);
	ImGui::ColorEdit4("Color One", glm::value_ptr(m_colorOne));
	ImGui::ColorEdit4("Color Two", glm::value_ptr(m_colorTwo));
	if (ImGui::Button("Reset Color and Position")) {
		m_gridPos = glm::vec3(-.5f,-0.5f,0.0f);
		m_colorOne = glm::vec4(0.8, 0.2, 0.3, 1.0);
		m_colorTwo = glm::vec4(0.2, 0.3, 0.8, 1.0);
	}
	ImGui::End();
}