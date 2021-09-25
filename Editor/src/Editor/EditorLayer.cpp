#include "EditorLayer.h"
#include <imgui/imgui.h>
#include "Editor/EditorApp.h"
namespace Stulu {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_sceneCamera(0.0, 85.0f,.1f,100.0f) {
		RenderCommand::setClearColor(glm::vec4(.0f));
		m_skybox = Skybox(CubeMap::create({
			"Stulu/assets/SkyBox/default/right.jpg",
			"Stulu/assets/SkyBox/default/left.jpg",
			"Stulu/assets/SkyBox/default/top.jpg",
			"Stulu/assets/SkyBox/default/bottom.jpg",
			"Stulu/assets/SkyBox/default/front.jpg",
			"Stulu/assets/SkyBox/default/back.jpg"
			}));
		cube = Model("assets/cube.obj");
		sphere = Model("assets/sphere.obj");
		shaderLib.load("Stulu/assets/Shaders/Reflective.glsl");
		shaderLib.load("Stulu/assets/Shaders/pbr.glsl");

		FrameBufferSpecs fspecs = FrameBufferSpecs();
		fspecs.width = Stulu::Application::get().getWindow().getWidth();
		fspecs.height = Stulu::Application::get().getWindow().getHeight();

		m_frameBuffer = FrameBuffer::create(fspecs);
	}

	void EditorLayer::onAttach() {
		GameObject go;
		Model car = Model("assets/car.glb");
		go = m_activeScene.createGameObject("Car");
		go.addComponent<ModelRendererComponent>(car, shaderLib.get("pbr"));

		class Controller : public Behavior {
		public:
			void start() {
				
			}
			void update(Timestep timestep) {
				getComponent<TransformComponent>().position.x += 8.0f * timestep;
			}
			void destroy() {

			}
		};

		go = m_activeScene.createGameObject("Light");
		go.getComponent<TransformComponent>().rotation.x = -145;
		go.getComponent<TransformComponent>().rotation.y = 30;
		go.addComponent<LightComponent>();
		
		go = m_activeScene.createGameObject("Sprite");
		go.getComponent<TransformComponent>().position.z = 12;
		go.addComponent<SpriteRendererComponent>(COLOR_PLASTIC_GREEN);

		go = m_activeScene.createGameObject("Camera");
		go.getComponent<TransformComponent>().position.z = 10;
		go.addComponent<CameraComponent>(CameraMode::Perspective);
		go.addComponent<NativeBehaviourComponent>().bind<Controller>();
	}

	void EditorLayer::onUpdate(Timestep timestep) {
		if (m_viewPortFocused && !m_runtime)
			m_sceneCamera.onUpdate(timestep);

		m_frameBuffer->bind();
		RenderCommand::clear();

		if(m_runtime)
			m_activeScene.onUpdateRuntime(timestep);
		else
			m_activeScene.onUpdateEditor(timestep, m_sceneCamera);
		
		m_skybox.draw();

		m_frameBuffer->unBind();


	}
	bool about = false;
	void EditorLayer::onImguiRender(Timestep timestep) {
		ImGui::DockSpaceOverViewport();
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New Scene")) {

				}
				if (ImGui::MenuItem("Exit")) {
					Application::exit(0);
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Settings")) {

				}
				ImGui::EndMenu();

			}
			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("About"))
					about = true;

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		if (about)
			if (ImGui::Begin("About - Stulu Engine")) {
				ImGui::Text("Stulu");
				ImGui::BulletText("https://github.com/stulu08/sengine");
				if (ImGui::Button("Close"))
					about = false;
				ImGui::End();

			}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		if (ImGui::Begin("View")) {
			
			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			m_viewPortPanelWidth = (uint32_t)viewportSize.x;
			m_viewPortPanelHeight = (uint32_t)viewportSize.y;
			
			//resizing
			FrameBufferSpecs FBspec = m_frameBuffer->getSpecs();
			if (m_viewPortPanelWidth > 0 && m_viewPortPanelHeight > 0 && (FBspec.width != m_viewPortPanelWidth || FBspec.height != m_viewPortPanelHeight)) {
				m_frameBuffer->Resize(m_viewPortPanelWidth, m_viewPortPanelHeight);
				m_sceneCamera.onResize((float)m_viewPortPanelWidth, (float)m_viewPortPanelHeight);
				m_activeScene.onViewportResize(m_viewPortPanelWidth, m_viewPortPanelHeight);
			}
			///////////////

			ImTextureID viewPortTexture = (void*)m_frameBuffer->getColorAttachmentRendereID();
			ImGui::Image(viewPortTexture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));

			m_viewPortFocused = ImGui::IsWindowFocused();
			m_viewPortHovered = ImGui::IsWindowHovered();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Debug");

		if (m_runtime)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha,.5f);
		else
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
		if (ImGui::Button("Play"))
			m_runtime = !m_runtime;
		ImGui::PopStyleVar();
		
		ImGui::End();
	}
	void EditorLayer::onEvent(Event& e) {
		if(!m_runtime)
			m_sceneCamera.onEvent(e);

		EventDispatcher dispacther(e);

		dispacther.dispatch<KeyDownEvent>(ST_BIND_EVENT_FN(EditorLayer::onShortCut));
	}
	bool EditorLayer::onShortCut(KeyDownEvent& e) {
		if (e.getRepeatCount() > 0)
			return false;

		bool control = Input::isKeyDown(Keyboard::LeftControl) || Input::isKeyDown(Keyboard::RightControl);
		bool shift = Input::isKeyDown(Keyboard::LeftShift) || Input::isKeyDown(Keyboard::RightShift);

		switch (e.getKeyCode())
		{
			case Keyboard::D:
				if (control) {
					
				}
				break;
		}
		return false;
	}
}