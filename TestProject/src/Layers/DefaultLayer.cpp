#include "DefaultLayer.h"
#include <imgui/imgui.cpp>

void DefaultLayer::onAttach() {
	std::shared_ptr<Stulu::VertexBuffer> vertexBuffer;
	std::shared_ptr<Stulu::IndexBuffer> indexBuffer;

	//triangle
	uint32_t indicies[3]{ 0,1,2 };
	float vertices[3 * 7] = {
		-.5f, -.5f, .0f,		0.0f, 0.0f, 1.0f, 1.0f,
		 .5f, -.5f, .0f,		1.0f, 0.0f, 0.0f, 1.0f,
		 .0f,  .5f, .0f,		0.0f, 1.0f, 0.0f, 1.0f
	};
	m_vertexArray.reset(Stulu::VertexArray::create());
	vertexBuffer.reset(Stulu::VertexBuffer::create(sizeof(vertices), vertices));
	Stulu::BufferLayout layout = {
		{ Stulu::ShaderDataType::Float3, "a_pos" },
		{ Stulu::ShaderDataType::Float4, "a_color" },
	};
	vertexBuffer->setLayout(layout);
	m_vertexArray->addVertexBuffer(vertexBuffer);
	indexBuffer.reset(Stulu::IndexBuffer::create(sizeof(indicies) / sizeof(uint32_t), indicies));
	m_vertexArray->setIndexBuffer(indexBuffer);
	std::string rainbowVertexShaderSource = R"(
		#version 460 core
		layout(location = 0) in vec3 a_pos;
		layout(location = 1) in vec4 a_color;
		uniform mat4 u_viewProjection;
		out vec4 v_color;
		void main(){
			v_color = a_color;
			gl_Position = u_viewProjection * vec4(a_pos, 1.0);
		})";
	std::string rainbowFragmentShaderSource = R"(
		#version 460 core
		layout(location = 0) out vec4 color;
		in vec4 v_color;
		void main(){
			color = v_color;
		})";
	m_rainbowShader.reset(Stulu::Shader::create(rainbowVertexShaderSource, rainbowFragmentShaderSource));

	//square
	uint32_t squareIndicies[6]{
		0,1,2,
		2,3,0
	};
	float squareVertices[3 * 4] = {
		-.75f,  .75f, .0f,
		-.75f, -.75f, .0f,
		 .75f, -.75f, .0f,
		 .75f,  .75f, .0f
	};
	m_squareVertexArray.reset(Stulu::VertexArray::create());
	vertexBuffer.reset(Stulu::VertexBuffer::create(sizeof(squareVertices), squareVertices));
	Stulu::BufferLayout squareLayout = {
		{ Stulu::ShaderDataType::Float3, "a_pos" },
	};
	vertexBuffer->setLayout(squareLayout);
	m_squareVertexArray->addVertexBuffer(vertexBuffer);
	indexBuffer.reset(Stulu::IndexBuffer::create(sizeof(squareIndicies) / sizeof(uint32_t), squareIndicies));
	m_squareVertexArray->setIndexBuffer(indexBuffer);
	std::string blueVertexShaderSource = R"(
		#version 460 core
		layout(location = 0) in vec3 a_pos;
		uniform mat4 u_viewProjection;
		void main(){
			gl_Position = u_viewProjection * vec4(a_pos, 1.0);
		})";
	std::string blueFragmentShaderSource = R"(
		#version 460 core
		layout(location = 0) out vec4 color;
		void main(){
			color = vec4(0.58, 0.61, 0.66, 1.0);
		})";
	m_blueShader.reset(Stulu::Shader::create(blueVertexShaderSource, blueFragmentShaderSource));


	//skybox

	std::vector<std::string> faces
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg",
	};
	//unsigned int cubemapTexture = Stulu::RenderAPI::loadCubemap(faces);

}
void DefaultLayer::onEvent(Stulu::Event& e) {

}
float speed_move = 8;
float speed_rotate = 64;
void DefaultLayer::onUpdate(Stulu::Timestep timestep) {
	Stulu::RenderCommand::setClearColor({ 0.15f, 0.15f, 0.15f, 1.0f });
	Stulu::RenderCommand::clear();

	glm::vec3 pos = m_camera.getPosition();
	float rotation = m_camera.getRotation();

	if (Stulu::Input::isKeyDown(KEY_W))
		pos.z += speed_move * timestep;
	else if (Stulu::Input::isKeyDown(KEY_S))
		pos.z -= speed_move * timestep;
	if (Stulu::Input::isKeyDown(KEY_A))
		pos.x -= speed_move * timestep;
	else if (Stulu::Input::isKeyDown(KEY_D))
		pos.x+= speed_move * timestep;
	if (Stulu::Input::isKeyDown(KEY_Q))
		pos.y -= speed_move * timestep;
	else if (Stulu::Input::isKeyDown(KEY_E))
		pos.y += speed_move * timestep;

	if (Stulu::Input::isKeyDown(KEY_LEFT))
		rotation += speed_rotate * timestep;
	else if (Stulu::Input::isKeyDown(KEY_RIGHT))
		rotation -= speed_rotate * timestep;;
	if (rotation >= 360 || rotation <= -360)
		rotation = (int)rotation % 360;

	m_camera.setPosition(pos);
	m_camera.setRotation(rotation);


	Stulu::Renderer::beginScene(m_camera);
	{
		Stulu::Renderer::submit(m_squareVertexArray, m_blueShader);

		Stulu::Renderer::submit(m_vertexArray, m_rainbowShader);
	}
	Stulu::Renderer::endScene();

}

void DefaultLayer::onImguiRender(Stulu::Timestep timestep) {


	drawRendererInfos();
	drawCameraInfos();
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
	ImGui::Text("Position:");
	ImGui::Text("x:%.2f y:%.2f z:%.2f ", m_camera.getPosition().x, m_camera.getPosition().y, m_camera.getPosition().z);
	ImGui::Text("Rotation: %.2f", m_camera.getRotation());
	ImGui::End();
}
void DefaultLayer::drawApplicationInfos(Stulu::Timestep timestep) {
	ImGui::Begin("Application");
	ImGui::Text("Deltatime: %.3fs(%dms)", (float)timestep, (int)timestep.getMilliseconds());
	ImGui::Text("FPS: %.2f", 1.0f / timestep);
	ImGui::End();
}