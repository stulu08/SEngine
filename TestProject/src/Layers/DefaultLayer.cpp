#include "DefaultLayer.h"
#include <imgui/imgui.cpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Platform/OpenGL/OpenGLShader.h>
#include <glm/glm/gtc/type_ptr.hpp>


void DefaultLayer::onAttach() {
	Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
	Stulu::Ref<Stulu::IndexBuffer> indexBuffer;

	//square
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
	m_squareVertexArray.reset(Stulu::VertexArray::create());
	vertexBuffer.reset(Stulu::VertexBuffer::create(sizeof(squareVertices), squareVertices));
	Stulu::BufferLayout squareLayout = {
		{ Stulu::ShaderDataType::Float3, "a_pos" },
		{ Stulu::ShaderDataType::Float2, "a_texCoord" },
	};
	vertexBuffer->setLayout(squareLayout);
	m_squareVertexArray->addVertexBuffer(vertexBuffer);
	indexBuffer.reset(Stulu::IndexBuffer::create(sizeof(squareIndicies) / sizeof(uint32_t), squareIndicies));
	m_squareVertexArray->setIndexBuffer(indexBuffer);

	m_colorShader.reset(Stulu::Shader::create("assets/Shaders/color.glsl"));
	m_texture = Stulu::Texture2D::create("assets/logo.png");
	m_textureShader.reset(Stulu::Shader::create("assets/Shaders/texture.sshader", true));

	std::dynamic_pointer_cast<Stulu::OpenGLShader>(m_textureShader)->bind();
	std::dynamic_pointer_cast<Stulu::OpenGLShader>(m_textureShader)->uploadIntUniform("u_color", 0);

}
void DefaultLayer::onEvent(Stulu::Event& e) {

}
float speed_move = 8;
float speed_rotate = 64;
void DefaultLayer::onUpdate(Stulu::Timestep timestep) {
	Stulu::RenderCommand::setClearColor(m_clearColor);
	Stulu::RenderCommand::clear();
	//camera movement
	{
		if (Stulu::Input::isKeyDown(KEY_W))
			m_camerpos.y += speed_move * timestep;
		else if (Stulu::Input::isKeyDown(KEY_S))
			m_camerpos.y -= speed_move * timestep;
		if (Stulu::Input::isKeyDown(KEY_A))
			m_camerpos.x -= speed_move * timestep;
		else if (Stulu::Input::isKeyDown(KEY_D))
			m_camerpos.x += speed_move * timestep;
		if (Stulu::Input::isKeyDown(KEY_Q))
			m_camerpos.z -= speed_move * timestep;
		else if (Stulu::Input::isKeyDown(KEY_E))
			m_camerpos.z += speed_move * timestep;
		if (Stulu::Input::isKeyDown(KEY_LEFT))
			m_camerarotation += speed_rotate * timestep;
		else if (Stulu::Input::isKeyDown(KEY_RIGHT))
			m_camerarotation -= speed_rotate * timestep;;
		if (m_camerarotation >= 360 || m_camerarotation <= -360)
			m_camerarotation = (int)m_camerarotation % 360;

		m_camera.setPosition(m_camerpos);
		m_camera.setRotation(m_camerarotation);
	}

	Stulu::Renderer::beginScene(m_camera);
	{
		{
			glm::mat4 gridScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
			for (int x = 0; x < 10; x++) {
				for (int y = 0; y < 10; y++) {
					glm::vec3 gridPos = glm::vec3(x * 0.11f, y * 0.11f, 0.0f) + m_gridPos;
					glm::mat4 gridTransform = glm::translate(glm::mat4(1.0f), gridPos) * gridScale;
					if (y % 2 == 0 ? x % 2 == 0 : x % 2 != 0)
						std::dynamic_pointer_cast<Stulu::OpenGLShader>(m_colorShader)->uploadFloat4Uniform("u_color", m_colorOne);
					else
						std::dynamic_pointer_cast<Stulu::OpenGLShader>(m_colorShader)->uploadFloat4Uniform("u_color", m_colorTwo);
					Stulu::Renderer::submit(m_squareVertexArray, m_colorShader, gridTransform);
				}

			}
		}
		{
			glm::mat4 squaretransform = glm::translate(glm::mat4(1.0f), m_squarePos) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
			m_texture->bind();
			Stulu::Renderer::submit(m_squareVertexArray, m_textureShader, squaretransform);
		}
	}
	Stulu::Renderer::endScene();

}

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
	ImGui::Begin("Camera", NULL, ImGuiWindowFlags_NoResize);
	ImGui::DragFloat3("Position", glm::value_ptr(m_camerpos), .1f);
	ImGui::DragFloat("Rotation", &m_camerarotation, .1f);
	ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor));
	if (ImGui::Button("Reset Color and Position")) {
		m_camerpos = glm::vec3(0.0f);
		m_camerarotation = 0.0f;
		m_clearColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
	}
		
	ImGui::End();
}
float totalfpscountrpausetime;
float deltaTimeForFpsCounter;
void DefaultLayer::drawApplicationInfos(Stulu::Timestep timestep) {
	totalfpscountrpausetime += timestep;
	if (totalfpscountrpausetime > .25f) {
		deltaTimeForFpsCounter = timestep;
		totalfpscountrpausetime = 0.0f;
	}

	ImGui::Begin("Application");
	ImGui::Text("Deltatime: %.3fs(%dms)", deltaTimeForFpsCounter, (int)deltaTimeForFpsCounter*1000);
	ImGui::Text("FPS: %.2f", 1.0f / deltaTimeForFpsCounter);
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