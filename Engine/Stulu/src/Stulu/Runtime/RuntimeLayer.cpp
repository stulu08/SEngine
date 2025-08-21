#include "st_pch.h"
#include "RuntimeLayer.h"
#include "Stulu.h"

#include "Stulu/Scripting/Managed/Bindings/Core/Input.h"

namespace Stulu {
	RuntimeLayer::RuntimeLayer(const std::string& startScene)
		: Layer("RuntimeLayer") {
		RenderCommand::setClearColor({ .0f, .0f, .0f, 1.0f });
		FrameBufferSpecs fspecs;
		fspecs.width = Stulu::Application::get().getWindow().getWidth();
		fspecs.height = Stulu::Application::get().getWindow().getHeight();
		m_frameBuffer = FrameBuffer::create(fspecs);

		OpenScene(startScene);
	}

	RuntimeLayer::~RuntimeLayer() {

	}

	void RuntimeLayer::onAttach() {
		Input::setEnabled(true);
		StuluBindings::Input::SetEnabled(true);
	}

	void RuntimeLayer::onUpdate(Timestep timestep) {
		m_activeScene->onUpdateRuntime();
		//draw texture to screen
		{
			m_activeScene->getRenderer()->GenSceneTexture(m_frameBuffer);

			const auto& shader = Resources::FullscreenShader();
			const auto& vao = Resources::getFullscreenVA();
			const float z = -1.0f;

			Renderer::getBuffer(BufferBinding::Model)->setData(&z, sizeof(float));

			RenderCommand::setViewport(0, 0, m_frameBuffer->getColorAttachment()->getWidth(), m_frameBuffer->getColorAttachment()->getHeight());
			RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));
			RenderCommand::clear();

			m_frameBuffer->getColorAttachment()->bind(0);
			shader->bind();
			RenderCommand::drawIndexed(vao, 0);
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
		m_frameBuffer->resize(e.getWidth(), e.getHeight());
		return false;
	}
	void RuntimeLayer::OpenScene(const std::string& path) {
		if (path.empty()) {
			newScene();
			return;
		}
		Ref<Scene> nScene = createRef<Scene>();
		SceneSerializer ss(nScene.get());
		if (ss.Deserialze(path)) {

			if (m_activeScene)
				m_activeScene->onRuntimeStop();

			m_activeScene = nScene;
			onResize(WindowResizeEvent(Application::get().getWidth(), Application::get().getHeight()));
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
		onResize(WindowResizeEvent(Application::get().getWidth(), Application::get().getHeight()));
		onRuntimeStart();
		ST_TRACE("New Scene loaded");
	}
	void RuntimeLayer::onRuntimeStart() {
		Time::Scale = 1.0f;
		StuluBindings::SetCurrentRegistry(m_activeScene.get());
		m_activeScene->onRuntimeStart();
	}
	void RuntimeLayer::onRuntimeStop() {
		m_activeScene->onRuntimeStop();
	}
}