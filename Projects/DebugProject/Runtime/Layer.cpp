#include "Layer.h"
#include "App.h"

#include <Stulu/Scripting/Managed/Bindings/Core/Input.h>

namespace Stulu {

	RuntimeLayer::RuntimeLayer()
		: Layer("RuntimeLayer") {
		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));
		FrameBufferSpecs fspecs;
		fspecs.width = Stulu::Application::get().getWindow().getWidth();
		fspecs.height = Stulu::Application::get().getWindow().getHeight();
		m_fbDrawData.m_sceneFBo = FrameBuffer::create(fspecs);
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
	}

	RuntimeLayer::~RuntimeLayer() {

	}

	void RuntimeLayer::onAttach() {
		YAML::Node node = YAML::LoadFile("Stulu/app");
		if (node) {
			UUID sceneID = node["Start Scene"].as<uint64_t>();
			OpenScene(AssetsManager::get(sceneID).path);
		}
		else {
			for (auto asset : AssetsManager::getAllByType(AssetType::Scene)) {
				OpenScene(AssetsManager::get(asset).path);
				return;
			}
		}
		Input::setEnabled(true);
		StuluBindings::Input::s_enabled = true;

	}

	void RuntimeLayer::onUpdate(Timestep timestep) {
		m_activeScene->onUpdateRuntime(timestep);
		//draw texture to screen
		{
			m_activeScene->getRenderer()->GenSceneTexture(m_fbDrawData.m_sceneFBo);
			RenderCommand::setViewport(0, 0, m_fbDrawData.m_sceneFBo->getColorAttachment()->getWidth(), m_fbDrawData.m_sceneFBo->getColorAttachment()->getHeight());
			RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));
			RenderCommand::clear();
			m_fbDrawData.m_sceneFBo->getColorAttachment()->bind(0);
			m_fbDrawData.m_quadShader->bind();
			m_fbDrawData.m_quadVertexArray->bind();
			RenderCommand::drawIndexed(m_fbDrawData.m_quadVertexArray);
		}
	}
	void RuntimeLayer::onEvent(Event& e) {
		m_activeScene->onEvent(e);
		EventDispatcher dispacther(e);
		dispacther.dispatch<WindowCloseEvent>(ST_BIND_EVENT_FN(RuntimeLayer::onApplicationQuit));
		dispacther.dispatch<WindowResizeEvent>(ST_BIND_EVENT_FN(RuntimeLayer::onResize));
	}
	bool RuntimeLayer::onApplicationQuit(WindowCloseEvent& e) {
		onRuntimeStop();
		return false;
	}
	bool RuntimeLayer::onResize(WindowResizeEvent& e) {
		m_activeScene->onViewportResize(e.getWidth(), e.getHeight());
		m_fbDrawData.m_sceneFBo->resize(e.getWidth(), e.getHeight());
		return false;
	}
	void RuntimeLayer::OpenScene(const std::string& path) {
		if (path.empty()) {
			newScene();
			return;
		}
		Ref<Scene> nScene = createRef<Scene>();
		SceneSerializer ss(nScene);
		if (ss.deSerialze(path)) {
			
			if(m_activeScene)
				m_activeScene->onRuntimeStop();

			m_activeScene = nScene;
			onResize(WindowResizeEvent(Application::getWidth(), Application::getHeight()));
			Scene::setActiveScene(m_activeScene.get());
			onRuntimeStart();
			ST_TRACE("Opened Scene {0}", path);
		}
		else {
			newScene();
		}

	}
	void RuntimeLayer::newScene() {
		if (m_activeScene)
			m_activeScene->onRuntimeStop();
		m_activeScene = createRef<Scene>();
		onResize(WindowResizeEvent(Application::getWidth(), Application::getHeight()));
		Scene::setActiveScene(m_activeScene.get());
		onRuntimeStart();
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