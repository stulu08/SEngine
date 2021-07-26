#include "st_pch.h"
#include "Application.h"

#include "Log.h"

#include "Renderer/Buffer.h"
#include <glad/glad.h>

namespace Stulu {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::s_instance = nullptr;

	Application::Application() {
		s_instance = this;
		m_window = std::unique_ptr<Window>(Window::create());
		m_window->setEventCallback(BIND_EVENT_FN(onEvent));

		m_imguiLayer = new ImGuiLayer();
		pushOverlay(m_imguiLayer);


		std::shared_ptr<VertexBuffer> vertexBuffer;
		std::shared_ptr<IndexBuffer> indexBuffer;

		//triangle
		uint32_t indicies[3]{ 0,1,2 };
		float vertices[3 * 7] = {
			-.5f, -.5f, .0f,		0.0f, 0.0f, 1.0f, 1.0f,
			 .5f, -.5f, .0f,		1.0f, 0.0f, 0.0f, 1.0f,
			 .0f,  .5f, .0f,		0.0f, 1.0f, 0.0f, 1.0f
		};
		m_vertexArray.reset(VertexArray::create());
		vertexBuffer.reset(VertexBuffer::create(sizeof(vertices), vertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_pos" },
			{ ShaderDataType::Float4, "a_color" },
		};
		vertexBuffer->setLayout(layout);
		m_vertexArray->addVertexBuffer(vertexBuffer);
		indexBuffer.reset(IndexBuffer::create(sizeof(indicies) / sizeof(uint32_t), indicies));
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
		m_rainbowShader.reset(Shader::create(rainbowVertexShaderSource, rainbowFragmentShaderSource));

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
		m_squareVertexArray.reset(VertexArray::create());
		vertexBuffer.reset(VertexBuffer::create(sizeof(squareVertices), squareVertices));
		BufferLayout squareLayout = {
			{ ShaderDataType::Float3, "a_pos" },
		};
		vertexBuffer->setLayout(squareLayout);
		m_squareVertexArray->addVertexBuffer(vertexBuffer);
		indexBuffer.reset(IndexBuffer::create(sizeof(squareIndicies) / sizeof(uint32_t), squareIndicies));
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
			color = vec4(0.1, 0.2, 0.8, 1.0);
		})";
		m_blueShader.reset(Shader::create(blueVertexShaderSource, blueFragmentShaderSource));

	}
	Application::~Application() {
		
	}
	void Application::pushLayer(Layer* layer) { m_layerStack.pushLayer(layer); }
	void Application::pushOverlay(Layer* layer) { m_layerStack.pushOverlay(layer); }

	void Application::onEvent(Event& e) {
		EventDispatcher dispacther(e);
		dispacther.dispacth<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*--it)->onEvent(e);
			if (e.handled)
				break;
		}

	}
	void Application::run() {
		while (m_runnig) {
			glClearColor(0.15f, 0.15f, 0.15f, .3f);
			glClear(GL_COLOR_BUFFER_BIT);



			m_blueShader->bind();
			m_squareVertexArray->bind();
			glDrawElements(GL_TRIANGLES, m_squareVertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);


			m_rainbowShader->bind();
			m_vertexArray->bind();
			glDrawElements(GL_TRIANGLES, m_vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);



			for (Layer* layer : m_layerStack)
				layer->onUpdate();
			m_imguiLayer->Begin();
			for (Layer* layer : m_layerStack)
				layer->onImguiRender();
			m_imguiLayer->End();
			m_window->onUpdate();
		}
	}
	bool Application::onWindowClose(WindowCloseEvent& e) {
		m_runnig = false;
		return m_runnig;
	}
}