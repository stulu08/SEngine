#include "Skybox.h"

Skybox::Skybox() {
	load();
}

Skybox::Skybox(std::vector<std::string> faces) {
	m_texture = Stulu::CubeMap::create(faces);
	load();
}
Skybox::Skybox(Stulu::Ref<Stulu::CubeMap> texture) {
	m_texture = texture;
	load();
}

void Skybox::load() {
	ST_PROFILING_FUNCTION();
	m_shader = Stulu::Shader::create("Stulu/assets/Shaders/SkyBox.glsl");
	m_shader->bind();
	m_shader->setInt("u_skyBox", 0);
	float skyboxVertices[6 * 3 * 6] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	m_vertexArray = Stulu::VertexArray::create();
	Stulu::Ref<Stulu::VertexBuffer> vertexBuffer = Stulu::VertexBuffer::create(sizeof(skyboxVertices), skyboxVertices);
	Stulu::BufferLayout layout = {
			{ Stulu::ShaderDataType::Float3, "a_pos" },
	};
	vertexBuffer->setLayout(layout);
	m_vertexArray->addVertexBuffer(vertexBuffer);
}

void Skybox::draw(const Stulu::Camera& cam) {
	ST_PROFILING_FUNCTION();
	Stulu::RenderCommand::setDepthFunc(true);
	m_texture->bind();
	m_shader->bind();
	glm::mat4 _view = glm::mat4(glm::mat3(cam.getViewMatrix()));
	m_shader->setMat4("u_viewProjection", cam.getProjectionMatrix() * _view);
	m_shader->setMat4("u_transform", glm::mat4(1.0f));
	m_vertexArray->bind();

	Stulu::RenderCommand::drawPrimitiveArray(m_indexCount);
	Stulu::RenderCommand::setDepthFunc(false);
}
