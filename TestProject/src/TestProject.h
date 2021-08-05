#pragma once
#include <Stulu.h>
#include <Layers/DefaultLayer.h>



class ExampleLayer : public Stulu::Layer {
public:
	ExampleLayer()
		:Layer("Name") {};
	void onAttach() override {
		uint32_t indices[3]{
			0,1,2,
		};
		float vertices[3 * 3] = {
			-0.5f, -0.5f, .0f,
			 0.5f, -0.5f, .0f,
			  .0f,  0.5f, .0f
		};
		m_triangleVertexArray = Stulu::VertexArray::create();
		Stulu::Ref<Stulu::VertexBuffer> vertexBuffer = Stulu::VertexBuffer::create(sizeof(vertices), vertices);
		Stulu::Ref<Stulu::IndexBuffer> indexBuffer = Stulu::IndexBuffer::create(sizeof(indices) / sizeof(uint32_t), indices);

		Stulu::BufferLayout layout = {
			{ Stulu::ShaderDataType::Float3, "a_pos" },
		};
		vertexBuffer->setLayout(layout);

		m_triangleVertexArray->addVertexBuffer(vertexBuffer);
		m_triangleVertexArray->setIndexBuffer(indexBuffer);
	}
	void onUpdate(Stulu::Timestep timestep) override {
		//render
		Stulu::RenderCommand::setClearColor({0.0f, 0.0f, 0.0f, 0.0f});
		Stulu::RenderCommand::clear();

		m_triangleVertexArray->bind();
		Stulu::RenderCommand::drawIndex(m_triangleVertexArray);
	}
private:
	Stulu::Ref<Stulu::VertexArray> m_triangleVertexArray;
};

class TestProject : public Stulu::Application {
public:
	TestProject() {
		//pushLayer(defaultLayer);
		pushLayer(new ExampleLayer());
		getWindow().setWindowIcon("assets/Logo/engine-icon-bg.png");
	}
	~TestProject() {

	}

	DefaultLayer* defaultLayer = new DefaultLayer();
};

