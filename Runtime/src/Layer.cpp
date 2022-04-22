#include "Layer.h"
#include "App.h"

namespace Stulu {

	RuntimeLayer::RuntimeLayer()
		: Layer("RuntimeLayer") {
		ST_PROFILING_FUNCTION();

		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));

		{
			Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
			Stulu::Ref<Stulu::IndexBuffer> indexBuffer;
			m_fbDrawData.m_quadVertexArray = Stulu::VertexArray::create();
			float vertices[20]{
				-1.0f,-1.0f,.0f, 0.0f,0.0f,
				1.0f,-1.0f,.0f, 1.0f,0.0f,
				1.0f,1.0f,.0f, 1.0f,1.0f,
				-1.0f,1.0f,.0f, 0.0f,1.0f,
			};
			vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(20 * sizeof(float)), vertices);
			vertexBuffer->setLayout({
		{ Stulu::ShaderDataType::Float3, "a_pos" },
		{ Stulu::ShaderDataType::Float2, "a_texCoord" },
				});
			m_fbDrawData.m_quadVertexArray->addVertexBuffer(vertexBuffer);
			uint32_t indices[6]{
				0,1,2,
				2,3,0
			};
			indexBuffer = Stulu::IndexBuffer::create((uint32_t)6, indices);
			m_fbDrawData.m_quadVertexArray->setIndexBuffer(indexBuffer);
			m_fbDrawData.m_quadShader = Shader::create("quadFullScreen", R"(
		#version 460
		layout (location = 0) in vec3 a_pos;
		layout (location = 1) in vec2 a_texCoords;
		out vec2 v_tex;
		void main()
		{
			v_tex=a_texCoords;
			gl_Position=vec4(a_pos, 1.0);
		}
		)", R"(
		#version 460
		in vec2 v_tex;
		layout (binding = 0) uniform sampler2D texSampler;
		out vec4 color;
		void main()
		{
			color = vec4(texture2D(texSampler, v_tex).rgb, 1.0);
		}
		)");
		}
		Resources::load();
	}

	RuntimeLayer::~RuntimeLayer() {

	}

	void RuntimeLayer::onAttach() {
		ST_PROFILING_FUNCTION();
		newScene();
		onRuntimeStart();
	}

	void RuntimeLayer::onUpdate(Timestep timestep) {
		ST_PROFILING_FUNCTION();

		m_activeScene->onUpdateRuntime(timestep);
		//draw scene to texture
		GameObject cO = m_activeScene->getMainCamera();
		if (cO) {
			RenderCommand::setViewport(0, 0, Application::getWidth(), Application::getHeight());
			RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));
			RenderCommand::clear();
			cO.getComponent<CameraComponent>().getTexture()->bind(0);
			m_fbDrawData.m_quadShader->bind();
			m_fbDrawData.m_quadVertexArray->bind();
			RenderCommand::drawIndexed(m_fbDrawData.m_quadVertexArray);
		}

	}
	void RuntimeLayer::onEvent(Event& e) {
		ST_PROFILING_FUNCTION();
		EventDispatcher dispacther(e);
		dispacther.dispatch<WindowCloseEvent>(ST_BIND_EVENT_FN(RuntimeLayer::onApplicationQuit));
		dispacther.dispatch<WindowResizeEvent>(ST_BIND_EVENT_FN(RuntimeLayer::onResize));
	}
	bool RuntimeLayer::onApplicationQuit(WindowCloseEvent& e) {
		onRuntimeStop();
		m_activeScene->onApplicationQuit();
		return false;
	}
	bool RuntimeLayer::onResize(WindowResizeEvent& e) {
		m_activeScene->onViewportResize(e.getWidth(), e.getHeight());
		return false;
	}
	void RuntimeLayer::OpenScene(const std::string& path) {
		ST_PROFILING_FUNCTION();
		if (path.empty()) {
			return;
		}
		m_activeScene->onRuntimeStop();
		Ref<Scene> nScene = createRef<Scene>();
		SceneSerializer ss(nScene);
		if (ss.deSerialze(path)) {
			m_activeScene = nScene;
			m_activeScene->onViewportResize(Application::getWidth(), Application::getHeight());
			ST_TRACE("Opened Scene {0}", path);
		}
		else {
			newScene();
		}

	}
	void RuntimeLayer::newScene() {
		ST_PROFILING_FUNCTION();
		m_activeScene = createRef<Scene>();
		m_activeScene->onViewportResize(Application::getWidth(), Application::getHeight());
		ST_TRACE("New Scene loaded");
	}
	void RuntimeLayer::onRuntimeStart() {
		Time::Scale = 1.0f;
		m_activeScene->onRuntimeStart();
	}
	void RuntimeLayer::onRuntimeStop() {
		m_activeScene->onRuntimeStop();
	}
}