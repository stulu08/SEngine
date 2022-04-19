#include "EditorLayer.h"
#include "Editor/EditorApp.h"
#include "Editor/DiscordRPC.h"

#include <ImGuizmo/ImGuizmo.h>

#include "Stulu/ScriptCore/Bindings/Input.h"

namespace Stulu {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_sceneCamera(0.0, 85.0f, .001f, 1000.0f) {
		ST_PROFILING_FUNCTION();

		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));

		FrameBufferSpecs fspecs = FrameBufferSpecs();
		fspecs.width = Stulu::Application::get().getWindow().getWidth();
		fspecs.height = Stulu::Application::get().getWindow().getHeight();
		m_sceneCamera.getCamera()->getFrameBuffer()->getSpecs() = fspecs;
		{
			m_fbDrawData.m_framebuffer = FrameBuffer::create(fspecs);
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
		m_assetBrowser = AssetBrowserPanel(EditorApp::getProject().assetPath);
		Resources::load();
		StyleEditor::init();
		Previewing::init();
		DiscordRPC::setDetails("Idling in " + getEditorProject().name);

		//load licenses
		for (auto fileDir : std::filesystem::directory_iterator("Licenses")) {
			if (fileDir.is_directory())
				continue;

			std::ifstream inStream(fileDir.path().string(), std::ios::in | std::ios::binary);
			if (inStream) {
				std::string result;
				inStream.seekg(0, std::ios::end);
				result.resize(inStream.tellg());
				inStream.seekg(0, std::ios::beg);
				inStream.read(&result[0], result.size());
				inStream.close();
				m_licenses[fileDir.path().filename().string()] = result;
			}
			else
				CORE_ERROR("Could not open File: {0}", fileDir.path().string());
		}
	}

	EditorLayer::~EditorLayer() {

	}

	void EditorLayer::onAttach() {
		ST_PROFILING_FUNCTION();
		newScene();
		loadPanelConfig();

#if 0
		GameObject object = m_activeScene->createGameObject("Cube");
		object.addComponent<MeshFilterComponent>().mesh = Resources::getCubeMeshAsset();

		class RigidBodyForceTest : public Behavior {
		public:
			Keyboard::KeyCode keycode = Keyboard::Space;
			float force = 8.0f;

			virtual void onAwake() override {
				addComponent<BoxColliderComponent>();
				addComponent<RigidbodyComponent>();
			}
			virtual void onUpdate() override {
				if (Input::isKeyDown(keycode))
					getComponent<RigidbodyComponent>().addForce(TRANSFORM_UP_DIRECTION, RigidbodyComponent::ForceMode::Impulse);
			}
			virtual void uiFunc() override {
				ComponentsRender::drawFloatControl("Force", force);
				ComponentsRender::drawIntControl("Key", (int&)keycode);
			}
		};
		object.addComponent<NativeBehaviourComponent>().bind<RigidBodyForceTest>();
#endif
	}

	void EditorLayer::onUpdate(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		if (!ImGuizmo::IsUsing()) {
			Input::s_enabled = m_sceneViewport.hovered || m_sceneViewport.focused;
			m_sceneCamera.updateMove(timestep);
			m_sceneCamera.onUpdate(timestep);
			m_activeScene->updateTransform(m_sceneCamera.getTransform());
			Input::s_enabled = true;
		}


		if (s_runtime) {
			m_activeScene->onUpdateRuntime(timestep);
			//draw scene to texture
			GameObject cO = m_activeScene->getMainCamera();
			if (cO) {
				RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));
				m_fbDrawData.m_framebuffer->bind();
				RenderCommand::clear();
				cO.getComponent<CameraComponent>().getTexture()->bind(0);
				m_fbDrawData.m_quadShader->bind();
				m_fbDrawData.m_quadVertexArray->bind();
				RenderCommand::drawIndexed(m_fbDrawData.m_quadVertexArray);
				m_fbDrawData.m_framebuffer->unbind();
			}
		}

		m_activeScene->onUpdateEditor(timestep, m_sceneCamera);
	}
	void EditorLayer::onImguiRender(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		ImGui::DockSpaceOverViewport();

		m_editorScene->updateAssemblyScripts("onDrawEditorGUI()");

		drawMenuBar();
		if (m_showGameViewport) {
			auto cam = m_activeScene->getMainCamera();
			if (cam) {
				m_gameViewport.draw(m_fbDrawData.m_framebuffer, &m_showGameViewport);
				if (m_gameViewport.width > 0 && m_gameViewport.height > 0 && (m_activeScene->m_viewportWidth != m_gameViewport.width || m_activeScene->m_viewportHeight != m_gameViewport.height)) {
					m_activeScene->onViewportResize(m_gameViewport.width, m_gameViewport.height);
					m_fbDrawData.m_framebuffer->resize(m_gameViewport.width, m_gameViewport.height);
				}
			}
			else {
				m_gameViewport.draw(nullptr, &m_showGameViewport);
			}
		}
		if (m_showSceneViewport) {
			m_sceneViewport.draw(m_sceneCamera, &m_showSceneViewport);
		}
		Application::get().getImGuiLayer()->blockEvents(
		!(m_gameViewport.focused || m_sceneViewport.hovered || m_sceneViewport.focused)
		);
		StuluBindings::Input::s_enabled = m_gameViewport.focused;

		if (m_showProfiling) {
			m_profilingPanel.draw(timestep, &m_showProfiling);
		}
		if (m_showStyleEditor) {
			StyleEditor::drawStyleEditor(&ImGui::GetStyle(), &m_showStyleEditor);
		}
		if (m_showHierarchy) {
			m_editorHierarchy.render(&m_showHierarchy);
		}
		if (m_showInspector) {
			if (!m_inspectorPanel.render(m_editorHierarchy.getCurrentObject(), &m_showInspector)) {
				m_editorHierarchy.setSelectedGameObject(GameObject::null);
			}
		}
		if (m_showAssetBrowser) {
			m_assetBrowser.render(&m_showAssetBrowser);
		}
		if (m_showLicensesWindow) {
			drawLicenseWindow();
		}
		if (m_showSceneSettingsPanel) {
			if (ImGui::Begin("Scene Settings", &m_showSceneSettingsPanel)) {
				SceneData& data = m_activeScene->m_data;
				ComponentsRender::drawFloatSliderControl("Exposure", data.toneMappingExposure, .0f, 5.0f);
				ComponentsRender::drawFloatSliderControl("Gamma", data.gamma, .0f, 5.0f);
				ComponentsRender::drawFloatSliderControl("Enviroment Lod", data.env_lod, .0f, 5.0f);
				if (ComponentsRender::drawBoolControl("HDR", data.framebuffer16bit)) {
					m_fbDrawData.m_framebuffer->getSpecs().textureFormat = ((data.framebuffer16bit ? TextureSettings::Format::RGBA16F : TextureSettings::Format::RGBA));
					m_fbDrawData.m_framebuffer->invalidate();
				}
				if (ImGui::TreeNodeEx("Physics")) {
					ComponentsRender::drawBoolControl("Enabled 3D Physics", data.enablePhsyics3D);
					ComponentsRender::drawVector3Control("Gravity", data.physicsData.gravity);
					ComponentsRender::drawFloatControl("Speed", data.physicsData.speed);
					ComponentsRender::drawFloatControl("Length", data.physicsData.length);
					ComponentsRender::drawIntSliderControl("Worker Threads", (int&)data.physicsData.workerThreads,0,16);
					ImGui::TreePop();
				}
			}
			ImGui::End();
		}

	}
	void EditorLayer::onRenderGizmo() {
		ST_PROFILING_FUNCTION();
		if (m_showSceneViewport) {
			if (ImGui::Begin("Scene", &m_showSceneViewport)) {
				Gizmo::Begin();
				GameObject selected = m_editorHierarchy.getCurrentObject();
				if (selected) {
					Gizmo::TransformEdit(selected.getComponent<TransformComponent>(), m_gizmoEditType);
				}
				if (selected != GameObject::null) {
					auto& tc = selected.getComponent<TransformComponent>();
					BoxColliderComponent boxCollider;
					if (selected.saveGetComponent<BoxColliderComponent>(boxCollider)) {
						glm::vec3 position = tc.worldPosition + boxCollider.offset;
						glm::vec3 scale = tc.worldScale * (boxCollider.size * 2.0f);

						Gizmo::drawOutlineCube(Math::createMat4(position, tc.worldRotation, scale),1.0f, glm::vec4(0, 1, 0, 1));
					}

					float scaleAdd = .02f;

					MeshFilterComponent meshFilter;
					m_sceneCamera.getCamera()->bindFrameBuffer();
					RenderCommand::setStencil(StencilMode::BeginDrawFromBuffer);
					RenderCommand::setCullMode(CullMode::BackAndFront);
					if (selected.hasComponent<MeshRendererComponent>() && selected.saveGetComponent<MeshFilterComponent>(meshFilter)) {
						if (meshFilter.mesh.hasMesh) {
							selected.getComponent<MeshRendererComponent>().m_enabledStencilBufferNextFrame = true;
							Renderer::submit(meshFilter.mesh.mesh->getVertexArray(),EditorResources::getOutlineShader(), Math::createMat4(tc.worldPosition,tc.worldRotation,tc.worldScale + glm::vec3(scaleAdd)));
							
						}
					}
					//each child ouline
					m_activeScene->m_registry.view<MeshFilterComponent, TransformComponent, MeshRendererComponent>().each([=](
						entt::entity go, MeshFilterComponent& meshFilter, TransformComponent& transform, MeshRendererComponent& meshRenderer
					) {
						if (transform.parent == selected) {
							GameObject object = GameObject{ go , m_activeScene.get() };
							if (meshFilter.mesh.hasMesh) {
								meshRenderer.m_enabledStencilBufferNextFrame = true;
								Renderer::submit(meshFilter.mesh.mesh->getVertexArray(), EditorResources::getOutlineShader(), Math::createMat4(transform.worldPosition, transform.worldRotation, transform.worldScale + glm::vec3(scaleAdd)));
							}
						}
					});
					RenderCommand::setCullMode(CullMode::BackAndFront);
					RenderCommand::setStencil(StencilMode::EndDrawFromBuffer);
					m_sceneCamera.getCamera()->unbindFrameBuffer();
				}
			}
			ImGui::End();
		}
	}
	void EditorLayer::onEvent(Event& e) {
		ST_PROFILING_FUNCTION();
		if(m_sceneViewport.focused && m_sceneViewport.hovered)
			m_sceneCamera.onEvent(e);
		if (e.getEventType() == EventType::WindowResize)
			m_sceneCamera.onResize((float)static_cast<WindowResizeEvent&>(e).getWidth(), (float)static_cast<WindowResizeEvent&>(e).getHeight());
		
		EventDispatcher dispacther(e);
		dispacther.dispatch<KeyDownEvent>(ST_BIND_EVENT_FN(EditorLayer::onShortCut));
		dispacther.dispatch<WindowCloseEvent>(ST_BIND_EVENT_FN(EditorLayer::onApplicationQuit));
	}
	void EditorLayer::drawMenuBar() {
		ST_PROFILING_FUNCTION();
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {

				if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
					newScene();
				}
				if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
					OpenScene();
				}
				if (ImGui::MenuItem("Save Scene", "Ctrl+S", false, !m_currentScenePath.empty())) {
					SaveScene(m_currentScenePath);
				}
				if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S")) {
					SaveScene();
				}
				if (ImGui::MenuItem("Exit", "Alt+F4")) {
					Application::exit(0);
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Settings")) {

				if (ImGui::MenuItem("VSync", "", getEditorApp().getWindow().isVSync())) {
					getEditorApp().getWindow().setVSync(!getEditorApp().getWindow().isVSync());
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				if (ImGui::MenuItem("Style Editor", (const char*)0, m_showStyleEditor)) {
					m_showStyleEditor = !m_showStyleEditor;
				}
				if (ImGui::MenuItem("Game Viewport", (const char*)0, m_showGameViewport)) {
					m_showGameViewport = !m_showGameViewport;
				}
				if (ImGui::MenuItem("Scene Viewport", (const char*)0, m_showSceneViewport)) {
					m_showSceneViewport = !m_showSceneViewport;
				}
				if (ImGui::MenuItem("Scene Hierachy", (const char*)0, m_showHierarchy)) {
					m_showHierarchy = !m_showHierarchy;
				}
				if (ImGui::MenuItem("Inspector", (const char*)0, m_showInspector)) {
					m_showInspector = !m_showInspector;
				}
				if (ImGui::MenuItem("Asset Browser", (const char*)0, m_showAssetBrowser)) {
					m_showAssetBrowser = !m_showAssetBrowser;
				}
				if (ImGui::MenuItem("Profiling", (const char*)0, m_showProfiling)) {
					m_showProfiling = !m_showProfiling;
				}
				if (ImGui::MenuItem("Scene Settings", (const char*)0, m_showSceneSettingsPanel)) {
					m_showSceneSettingsPanel = !m_showSceneSettingsPanel;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("About")) {
				if (ImGui::MenuItem("Licenses/3rd Party/Dependencies", (const char*)0, m_showLicensesWindow)) {
					m_showLicensesWindow = !m_showLicensesWindow;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		//toolbar
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

		float size = ImGui::GetWindowHeight() - 4.0f;
		ImVec4 icoColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);

		{
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size) + (size));
			Ref<Texture> tex = s_runtime ? EditorResources::getStopTexture() : EditorResources::getPlayTexture();
			if (ImGui::ImageButton(reinterpret_cast<void*>((uint64_t)tex->getRendererID()), { size, size }, { 0, 1 }, { 1, 0 }, 0, { 0,0,0,0 }, icoColor)) {
				if (s_runtime) {
					onRuntimeStop();
				}
				else {
					onRuntimeStart();
				}
			}
		}ImGui::SameLine();
		{
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size) - (size));
			Ref<Texture> tex = EditorResources::getPauseTexture();
			if (ImGui::ImageButton(reinterpret_cast<void*>((uint64_t)tex->getRendererID()), { size, size }, { 0, 1 }, { 1, 0 }, 0, { 0,0,0,0 }, icoColor)) {
				if (Time::Scale == 0.0f) {
					Time::Scale = 1.0f;
				}
				else {
					Time::Scale = 0.0f;
				}
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}
	void EditorLayer::drawLicenseWindow() {
		if (ImGui::Begin("Licenses/3rd Party/Dependencies", &m_showLicensesWindow)) {
			for (auto& [name, content] : m_licenses) {
				if (ImGui::CollapsingHeader(name.c_str())) {
					ImGui::Text(content.c_str());
				}
			}
		}
		ImGui::End();
	}
	bool EditorLayer::onShortCut(KeyDownEvent& e) {
		ST_PROFILING_FUNCTION();
		if (e.getRepeatCount() > 0)
			return false;

		bool control = Input::isKeyDown(Keyboard::LeftControl) || Input::isKeyDown(Keyboard::RightControl);
		bool shift = Input::isKeyDown(Keyboard::LeftShift) || Input::isKeyDown(Keyboard::RightShift);

		switch (e.getKeyCode())
		{

			case Keyboard::Q:
				m_gizmoEditType = GizmoTransformEditMode::None;
				DiscordRPC::setDetails("Idling in " + getEditorProject().name);
				break;
			case Keyboard::G:
				m_gizmoEditType = GizmoTransformEditMode::Translate;
				DiscordRPC::setDetails("Transforming in " + getEditorProject().name);
				break;
			case Keyboard::R:
				m_gizmoEditType = GizmoTransformEditMode::Rotate;
				DiscordRPC::setDetails("Rotation in " + getEditorProject().name);
				break;
			case Keyboard::S:
				if (control) {
					if (shift && !m_currentScenePath.empty())
						SaveScene(m_currentScenePath);
					else
						SaveScene();
					ST_TRACE("Saving Scene");
					break;
				}
				m_gizmoEditType = GizmoTransformEditMode::Scale;
				DiscordRPC::setDetails("Scaling in " + getEditorProject().name);
				break;
			case Keyboard::O:
				if (control) {
					OpenScene();
					ST_TRACE("Opening Scene");
				}
				break;
			case Keyboard::N:
				if (control) {
					newScene();
					ST_TRACE("New Scene");
				}
				break;
		}
		return false;
	}
	bool EditorLayer::onApplicationQuit(WindowCloseEvent& e) {
		DiscordRPC::shutdown();
		if (s_runtime)
			m_activeScene->onRuntimeStop();
		m_activeScene->onApplicationQuit();
		return false;
	}
	void EditorLayer::SaveScene() {
		ST_PROFILING_FUNCTION();
		std::string path = Platform::saveFile("Scene Files\0 * .scene\0", EditorApp::getProject().path.c_str());
		if (!path.empty())
			SaveScene(path);
	}
	void EditorLayer::OpenScene() {
		ST_PROFILING_FUNCTION();
		if (s_runtime) {
			m_activeScene->onRuntimeStop();
			s_runtime = false;
		}

		std::string path = Platform::openFile("Scene Files\0 * .scene\0", EditorApp::getProject().path.c_str());
		if (!path.empty())
			OpenScene(path);
	}
	void EditorLayer::SaveScene(const std::string& path) {
		ST_PROFILING_FUNCTION();
		if (path.empty()) {
			SaveScene();
		}
		m_currentScenePath = path;
		SceneSerializer ss(m_activeScene);
		ss.serialze(path);
		DiscordRPC::setState("Editing " + path.substr(path.find_last_of("/\\")+1, path.npos));
		ST_TRACE("Saved Scene {0}", path);
	}
	void EditorLayer::OpenScene(const std::string& path) {
		ST_PROFILING_FUNCTION();
		if (path.empty()) {
			OpenScene();
		}
		if (s_runtime) {
			m_activeScene->onRuntimeStop();
			s_runtime = false;
		}
		m_currentScenePath = path;
		Ref<Scene> nScene = createRef<Scene>();
		SceneSerializer ss(nScene);
		if (ss.deSerialze(path)) {
			m_editorScene = nScene;
			m_activeScene = m_editorScene;
			m_editorHierarchy.setScene(m_activeScene);
			m_activeScene->onViewportResize(m_sceneViewport.width, m_sceneViewport.height);
			ST_TRACE("Opened Scene {0}", path);
			DiscordRPC::setState("Editing " + path.substr(path.find_last_of("/\\") + 1, path.npos));
		}
		else {
			newScene();
		}

	}
	void EditorLayer::newScene() {
		ST_PROFILING_FUNCTION();
		m_currentScenePath = "";
		m_editorScene = createRef<Scene>();
		m_activeScene = m_editorScene;
		m_activeScene->onViewportResize(m_sceneViewport.width, m_sceneViewport.height);
		m_editorHierarchy.setScene(m_activeScene);
		DiscordRPC::setState("Editing a scene");
		ST_TRACE("New Scene loaded");
	}
	static std::unordered_map <MonoObjectInstance::MonoClassMember, void* > scriptFieldList;
	void EditorLayer::onRuntimeStart() {
		scriptFieldList.clear();
		Time::Scale = 1.0f;
		s_runtime = true;
		m_runtimeScene = Scene::copy(m_editorScene);
		m_activeScene = m_runtimeScene;

		m_activeScene->onRuntimeStart();
		m_fbDrawData.m_framebuffer->getSpecs().textureFormat = ((m_activeScene->m_data.framebuffer16bit ? TextureSettings::Format::RGBA16F : TextureSettings::Format::RGBA));
		m_fbDrawData.m_framebuffer->invalidate();

		GameObject selected = m_editorHierarchy.getCurrentObject();//in editor scene
		m_editorHierarchy.setScene(m_activeScene);
		if (selected)
			m_editorHierarchy.setSelectedGameObject(GameObject::getById(selected.getComponent<GameObjectBaseComponent>().uuid, m_activeScene.get()));
	}
	void EditorLayer::onRuntimeStop() {
		s_runtime = false;
		m_activeScene->onRuntimeStop();
		m_activeScene = m_editorScene;

		GameObject selected = m_editorHierarchy.getCurrentObject();//in runtime scene
		m_editorHierarchy.setScene(m_activeScene);
		if (selected)
			m_editorHierarchy.setSelectedGameObject(GameObject::getById(selected.getComponent<GameObjectBaseComponent>().uuid, m_activeScene.get()));

		m_runtimeScene = nullptr;
	}
	void EditorLayer::loadPanelConfig() {
		ST_PROFILING_FUNCTION();
		std::string file = getEditorProject().configPath + "/editor-panel-config.ini";
		if (!FileExists(file)) {
			savePanelConfig();
			return;
		}

		std::unordered_map<std::string, std::string> values;
		int count = 0;
		std::fstream stream(file, std::ios::in);
		std::string str;
		while (getline(stream, str)) {
			str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
			size_t seperator = str.find_first_of("=");
			std::string name = str.substr(0, seperator);
			std::string data = str.substr(seperator + 1, str.npos);
			values[name] = data;
			count++;
		}
		if (values.find("m_showStyleEditor") != values.end())
			m_showStyleEditor = std::stoi(values["m_showStyleEditor"]);
		if (values.find("m_showHierarchy") != values.end())
			m_showHierarchy = std::stoi(values["m_showHierarchy"]);
		if (values.find("m_showInspector") != values.end())
			m_showInspector = std::stoi(values["m_showInspector"]);
		if (values.find("m_showAssetBrowser") != values.end())
			m_showAssetBrowser = std::stoi(values["m_showAssetBrowser"]);
		if (values.find("m_showGameViewport") != values.end())
			m_showGameViewport = std::stoi(values["m_showGameViewport"]);
		if (values.find("m_showSceneViewport") != values.end())
			m_showSceneViewport = std::stoi(values["m_showSceneViewport"]);
		if (values.find("m_showProfiling") != values.end())
			m_showProfiling = std::stoi(values["m_showProfiling"]);
		if (values.find("m_showSceneSettingsPanel") != values.end())
			m_showSceneSettingsPanel = std::stoi(values["m_showSceneSettingsPanel"]);

		if (values.find("m_gameViewport.m_selectedSize") != values.end())
			m_gameViewport.m_selectedSize = std::stoi(values["m_gameViewport.m_selectedSize"]);
		if (values.find("m_gameViewport.m_customSizeX") != values.end())
			m_gameViewport.sizes[5].x = std::stof(values["m_gameViewport.m_customSizeX"]);
		if (values.find("m_gameViewport.m_customSizeY") != values.end())
			m_gameViewport.sizes[5].y = std::stof(values["m_gameViewport.m_customSizeY"]);
		if (values.find("m_gameViewport.zoom") != values.end())
			m_gameViewport.zoom = std::stof(values["m_gameViewport.zoom"]);

		if (values.find("setting.vsync") != values.end())
			getEditorApp().getWindow().setVSync(std::stoi(values["setting.vsync"]));
		if (values.find("setting.lastScene") != values.end()) {
			if (FileExists(values["setting.lastScene"])) {
				OpenScene(values["setting.lastScene"]);
			}
		}
			
		
		ST_TRACE("Loaded Editor panel config from {0}", file);
		return;
	}
	void EditorLayer::savePanelConfig() {
		ST_PROFILING_FUNCTION();
		std::string file = getEditorProject().configPath + "/editor-panel-config.ini";
		std::remove(file.c_str());
		std::fstream stream(file, std::ios::out);

		stream << "m_showStyleEditor=" << (int)m_showStyleEditor << "\n";
		stream << "m_showHierarchy=" << (int)m_showHierarchy << "\n";
		stream << "m_showInspector=" << (int)m_showInspector << "\n";
		stream << "m_showAssetBrowser=" << (int)m_showAssetBrowser << "\n";
		stream << "m_showGameViewport=" << (int)m_showGameViewport << "\n";
		stream << "m_showSceneViewport=" << (int)m_showSceneViewport << "\n";
		stream << "m_showProfiling=" << (int)m_showProfiling << "\n";
		stream << "m_showSceneSettingsPanel=" << (int)m_showSceneSettingsPanel << "\n";

		stream << "m_gameViewport.m_selectedSize=" << (int)m_gameViewport.m_selectedSize << "\n";
		stream << "m_gameViewport.m_customSizeX=" << (float)m_gameViewport.sizes[5].x << "\n";
		stream << "m_gameViewport.m_customSizeY=" << (float)m_gameViewport.sizes[5].y << "\n";
		stream << "m_gameViewport.zoom=" << (float)m_gameViewport.zoom << "\n";

		stream << "setting.vsync=" << (int)getEditorApp().getWindow().isVSync() << "\n";
		stream << "setting.lastScene=" << m_currentScenePath << "\n";


		stream.close();
		ST_TRACE("Saved Editor Panel config to {0}", file);
	}
}