#include "st_pch.h"
#include "Application.h"

#include "Log.h"

#include "Renderer/Buffer.h"
#include <glad/glad.h>

namespace Stulu {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::s_instance = nullptr;

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type)
		{
			case Stulu::ShaderDataType::Float:		return GL_FLOAT;
			case Stulu::ShaderDataType::Float2:		return GL_FLOAT;
			case Stulu::ShaderDataType::Float3:		return GL_FLOAT;
			case Stulu::ShaderDataType::Float4:		return GL_FLOAT;
			case Stulu::ShaderDataType::Int:		return GL_INT;
			case Stulu::ShaderDataType::Int2:		return GL_INT;
			case Stulu::ShaderDataType::Int3:		return GL_INT;
			case Stulu::ShaderDataType::Int4:		return GL_INT;
			case Stulu::ShaderDataType::Mat3:		return GL_FLOAT;
			case Stulu::ShaderDataType::Mat4:		return GL_FLOAT;
			case Stulu::ShaderDataType::Bool:		return GL_INT;
		}
		CORE_ASSERT(false, "Uknown ShaderDataType");
		return 0;
	}

	Application::Application() {
		s_instance = this;
		m_window = std::unique_ptr<Window>(Window::create());
		m_window->setEventCallback(BIND_EVENT_FN(onEvent));

		m_imguiLayer = new ImGuiLayer();
		pushOverlay(m_imguiLayer);




		glGenVertexArrays(1, &m_vertexArray);
		glBindVertexArray(m_vertexArray);
		
		float vertices[3 * 7] = {
			-.5f, -.5f, .0f,		0.0f, 0.0f, 1.0f, 1.0f,
			 .5f, -.5f, .0f,		1.0f, 0.0f, 0.0f, 1.0f,
			 .0f,  .5f, .0f,		0.0f, 1.0f, 0.0f, 1.0f
		};

		m_vertexBuffer.reset(VertexBuffer::create(sizeof(vertices), vertices));

		{
			BufferLayout layout = {
				{ ShaderDataType::Float3, "a_pos" },
				{ ShaderDataType::Float4, "a_color" },
			};
			m_vertexBuffer->setLayout(layout);
		}

		uint32_t index = 0;
		const auto& layout = m_vertexBuffer->getLayout();
		for (const auto& element : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index, element.getComponentCount(), 
				ShaderDataTypeToOpenGLBaseType(element.type), 
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(), (const void*) element.offset
			);
			index++;
		}




		uint32_t indicies[3]{ 0,1,2 };
		m_indexBuffer.reset(IndexBuffer::create(sizeof(indicies) / sizeof(uint32_t), indicies));



		std::string vertexSource = R"(
		#version 460 core

		layout(location = 0) in vec3 a_pos;
		layout(location = 1) in vec4 a_color;

		out vec3 v_pos;
		out vec4 v_color;

		void main(){
			v_pos = a_pos;
			v_color = a_color;
			gl_Position = vec4(a_pos, 1.0);
		}
)";
		std::string fragmentSource = R"(
		#version 460 core

		layout(location = 0) out vec4 color;

		in vec3 v_pos;
		in vec4 v_color;

		void main(){
			color = vec4(v_pos * 0.5 + 0.5, 1.0);
			color = v_color;
		}
)";

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