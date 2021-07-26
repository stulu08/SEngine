#include "DefaultLayer.h"

void DefaultLayer::onAttach() {
	std::shared_ptr<Stulu::VertexBuffer> vertexBuffer;
	std::shared_ptr<Stulu::IndexBuffer> indexBuffer;

	//triangle
	uint32_t indicies[3]{ 0,1,2 };
	float vertices[3 * 7] = {
		-.5f, -.5f, .0f,		0.6f, 0.2f, 0.8f, 1.0f,
		 .5f, -.5f, .0f,		0.2f, 0.4f, 0.7f, 1.0f,
		 .0f,  .5f, .0f,		0.8f, 0.7f, 0.3f, 1.0f
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
		out vec4 v_color;
		void main(){
			v_color = a_color;
			gl_Position = vec4(a_pos, 1.0);
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
		void main(){
			gl_Position = vec4(a_pos, 1.0);
		})";
	std::string blueFragmentShaderSource = R"(
		#version 460 core
		layout(location = 0) out vec4 color;
		void main(){
			color = vec4(0.1, 0.2, 0.7, 1.0);
		})";
	m_blueShader.reset(Stulu::Shader::create(blueVertexShaderSource, blueFragmentShaderSource));
}
void DefaultLayer::onEvent(Stulu::Event& e) {

}
void DefaultLayer::onUpdate() {
	Stulu::RenderCommand::setClearColor({ 0.15f, 0.15f, 0.15f, 1.0f });
	Stulu::RenderCommand::clear();

	Stulu::Renderer::beginScene();
	{
		m_blueShader->bind();
		Stulu::Renderer::submit(m_squareVertexArray);

		m_rainbowShader->bind();
		Stulu::Renderer::submit(m_vertexArray);
	}
	Stulu::Renderer::endScene();
}


