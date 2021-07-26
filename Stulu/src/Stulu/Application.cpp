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

		glGenVertexArrays(1, &m_vertexArray);
		glBindVertexArray(m_vertexArray);



		float vertices[3 * 3] = {
			-.5f, -.5f, .0f,
			 .5f, -.5f, .0f,
			 .0f,  .5f, .0f,
		};

		m_vertexBuffer.reset(VertexBuffer::create(sizeof(vertices), vertices));
		//m_vertexBuffer->bind();

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		uint32_t indicies[3]{ 0,1,2 };
		m_indexBuffer.reset(IndexBuffer::create(sizeof(indicies) / sizeof(uint32_t), indicies));



		std::string vertexSource = R"(
		#version 460 core
		layout(location = 0) in vec3 a_pos;
		out vec3 v_pos;
		void main(){
			v_pos = a_pos;
			gl_Position = vec4(a_pos, 1.0);
		})";
		std::string fragmentSource = R"(
		#version 460 core
		layout(location = 0) out vec4 color;
		in vec3 v_pos;
		void main(){
			color = vec4(v_pos * 0.5 + 0.5, 1.0);
		})";

		m_shader.reset(Shader::create(vertexSource, fragmentSource));
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

			m_shader->bind();
			glBindVertexArray(m_vertexArray);
			glDrawElements(GL_TRIANGLES, m_indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);

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