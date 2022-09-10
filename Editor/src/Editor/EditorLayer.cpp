#include "EditorLayer.h"
#include "Editor/EditorApp.h"
#include "Editor/DiscordRPC.h"

#include "Stulu/ScriptCore/Bindings/Input.h"

#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>


namespace Stulu {
	Ref<Texture2D> textTexture;
	void copyAllFilesFromDir(const std::string& from, const std::string& to);

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_sceneCamera(0.0, 85.0f, .001f, 1000.0f) {
		ST_PROFILING_FUNCTION();

		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));

		FrameBufferSpecs fspecs = FrameBufferSpecs();
		fspecs.width = Stulu::Application::get().getWindow().getWidth();
		fspecs.height = Stulu::Application::get().getWindow().getHeight();
		m_sceneCamera.getCamera()->getFrameBuffer()->getSpecs() = fspecs;

		fspecs.textureFormat = TextureSettings::Format::RGBA16F;
		m_sceneFrameBuffer = FrameBuffer::create(fspecs);

		m_assetBrowser = AssetBrowserPanel(EditorApp::getProject().assetPath);
		StyleEditor::init();
		Previewing::init();

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

#pragma region Debug
	bool terrainFallOff = true;
	uint32_t terrainSeed = Random::getInt(0, INT32_MAX);
	uint32_t terrainSize = 128;//in quads
	PerlinSettings noiseSettings = { 8, 2.0f, 25.0f, glm::vec2((float)terrainSize) };
	void generateDebugTerrain() {
		noiseSettings.size = glm::vec2((float)terrainSize);
		Math::setPerlinSeed(terrainSeed);
		auto& getYLevel = [&](const glm::vec2& pos) -> float {
			glm::vec2 origin = glm::vec2(terrainSize / 2.0f);
			float maxDistance = glm::distance(glm::vec2(.0f), origin * .9f);
			float noise = Math::simpleNosie(pos, noiseSettings);
			return terrainFallOff ? (noise * glm::clamp(1.0f - glm::distance(pos, origin) / maxDistance, 0.0f, 1.0f)) : noise;
		};
		auto& getColorByHeight = [&](float _y) -> glm::vec4 {
			float y = _y / noiseSettings.multiplier;
			if (y <= .4f) {//water

				if (y < .1f)//deep
					return { 0.25490f, 0.53333f, 1.00000f, 1.0f };
				if (y < .2f)//medium
					return { 0.39216f, 0.61961f, 1.00000f, 1.0f };
				if (y < .3f)//shallow
					return { 0.47059f, 0.66667f, 1.00000f, 1.0f };
				if (y < .4f)//beach water
					return { 0.57255f, 0.73333f, 1.00000f, 1.0f };
			}
			else {//land
				if (y < .5f)//beach
					return { 0.76078f, 0.69804f, 0.50196f, 1.0f };
				if (y < .7f)//normal
					return { 0.20392f, 0.54902f, 0.19216f, 1.0f };
				if (y < .8f)//hill
					return { 0.46667f, 0.46667f, 0.46667f, 1.0f };
				if (y < .9f)//mountain
					return { 0.33333f, 0.33333f, 0.33333f, 1.0f };
				if (y < 1.0f)//snow
					return { 1.0f, 1.0f, 1.0f, 1.0f };
			}
			return { 1.0f, 1.0f, 1.0f, 1.0f };
		};

		Ref<Mesh> mesh = createRef<Mesh>();


		std::vector<Vertex> vertices((terrainSize + 1) * (terrainSize + 1));
		std::vector<uint32_t> triangles(terrainSize * terrainSize * 6);

		for (uint32_t i = 0, z = 0; z <= terrainSize; z++)
		{
			for (uint32_t x = 0; x <= terrainSize; x++)
			{
				float y = getYLevel(glm::vec2(x, z));
				Vertex vertex;
				vertex.pos = glm::vec3(x, y, z);
				vertex.texCoords = glm::vec3(.0f);
				vertex.color = getColorByHeight(y);
				vertices[i] = vertex;
				i++;
			}
		}


		uint32_t vert = 0;
		uint32_t tris = 0;

		for (uint32_t z = 0; z < terrainSize; z++)
		{
			for (uint32_t x = 0; x < terrainSize; x++)
			{
				triangles[tris + 0] = vert + 0;
				triangles[tris + 1] = vert + terrainSize + 1;
				triangles[tris + 2] = vert + 1;
				triangles[tris + 3] = vert + 1;
				triangles[tris + 4] = vert + terrainSize + 1;
				triangles[tris + 5] = vert + terrainSize + 2;

				vert++;
				tris += 6;
			}

			vert++;
		}
		mesh->setVertices(vertices);
		mesh->setIndices(triangles);


		mesh->calculateNormals();
		mesh->recalculate();
		GameObject go = Scene::activeScene()->findGameObjectByName("Terrain");
		if (!go)
			go = Scene::activeScene()->createGameObject("Terrain");

		go.getComponent<TransformComponent>().position = { -(terrainSize / 2.0f), .0f, -(terrainSize / 2.0f) };
		go.saveAddComponent<MeshFilterComponent>().setMesh(mesh, "Terrain Mesh");
	}
#pragma endregion
#pragma region GUI
	Ref<Texture2D> debugTexture;
	void EditorLayer::onImguiRender(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		ImGui::DockSpaceOverViewport();
		drawMenuBar();

		SceneData& data = m_activeScene->m_data;
		if (m_showDebugWindow) {
			if (ImGui::Begin("Debug Window", &m_showDebugWindow)) {
				static uint32_t mip = 0;
				if (ImGui::TreeNodeEx("Display Only:")) {
					ComponentsRender::drawBoolBitFlagControl("Wireframe", ST_ShaderViewFlags_DisplayVertices, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Depth", ST_ShaderViewFlags_DisplayDepth, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Lighting", ST_ShaderViewFlags_DisplayLighting, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Normals", ST_ShaderViewFlags_DisplayNormal, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Tex Coords", ST_ShaderViewFlags_DisplayTexCoords, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Diffuse", ST_ShaderViewFlags_DisplayDiffuse, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Specular", ST_ShaderViewFlags_DisplaySpecular, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Roughness", ST_ShaderViewFlags_DisplayRoughness, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Ambient Occlusion", ST_ShaderViewFlags_DisplayAmbient, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Emission", ST_ShaderViewFlags_DisplayEmission, data.shaderFlags);
					ImGui::TreePop();
				}
				if (ImGui::TreeNodeEx("Terrain")) {
					ComponentsRender::drawUIntControl("Seed", terrainSeed); ImGui::SameLine();
					if (ImGui::Button("New")) {
						terrainSeed = Random::getInt(0, INT32_MAX);
					}
					ComponentsRender::drawUIntControl("Size", terrainSize);
					ComponentsRender::drawFloatControl("Frequency", noiseSettings.frequency);
					ComponentsRender::drawUIntControl("Octaves", noiseSettings.octaves);
					ComponentsRender::drawFloatControl("Max Height", noiseSettings.multiplier);
					ComponentsRender::drawBoolControl("Use FallOff", terrainFallOff);

					if (ImGui::Button("Generate"))
						generateDebugTerrain();
					ImGui::TreePop();
				}
			}
			ImGui::End();
		}
		if (m_showSceneSettingsPanel) {
			if (ImGui::Begin("Scene Settings", &m_showSceneSettingsPanel)) {
				if (ImGui::TreeNodeEx("Graphics")) {
					ComponentsRender::drawFloatSliderControl("Exposure", data.graphicsData.toneMappingExposure, .0f, 5.0f);
					ComponentsRender::drawFloatSliderControl("Gamma", data.graphicsData.gamma, .0f, 5.0f);
					if (ImGui::TreeNodeEx("Bloom")) {
						ComponentsRender::drawBoolControl("Bloom", data.graphicsData.bloom);
						ComponentsRender::drawFloatControl("Intensity", data.graphicsData.bloomIntensity, .001f);
						ComponentsRender::drawFloatControl("Treshold", data.graphicsData.bloomTreshold, .1f);
						ImGui::TreePop();
					}
					if (ImGui::TreeNodeEx("These still need work")) {
						ComponentsRender::drawFloatSliderControl("Env Lod", data.graphicsData.env_lod, 0.0f, 4.0f);
						ComponentsRender::drawBoolControl("Enviroment mapping", data.graphicsData.useReflectionMapReflections); ImGui::SameLine();
						ComponentsRender::drawHelpMarker("Reflections using a dynamicly generated CubeMap of the scene, is still in dev and u should not use it");
						ImGui::TreePop();
					}
					
					ImGui::TreePop();
				}
				if (ImGui::TreeNodeEx("Physics")) {
					ComponentsRender::drawBoolControl("3D Physics", data.enablePhsyics3D);
					ComponentsRender::drawVector3Control("Gravity", data.physicsData.gravity);
					ComponentsRender::drawFloatControl("Speed", data.physicsData.speed);
					ComponentsRender::drawFloatControl("Length", data.physicsData.length);
					ComponentsRender::drawIntSliderControl("Worker Threads", (int&)data.physicsData.workerThreads, 0, 16);
					ImGui::TreePop();
				}
			}
			ImGui::End();
		}

		if (m_showGameViewport) {
			if (s_runtime) {
				m_gameViewport.draw(m_sceneFrameBuffer, &m_showGameViewport);
			}
			else {
				m_gameViewport.draw(nullptr, &m_showGameViewport);
			}
			if (m_gameViewport.width > 0 && m_gameViewport.height > 0 && (m_activeScene->m_viewportWidth != m_gameViewport.width || m_activeScene->m_viewportHeight != m_gameViewport.height)) {
				m_activeScene->onViewportResize(m_gameViewport.width, m_gameViewport.height);
				m_sceneFrameBuffer->resize(m_gameViewport.width, m_gameViewport.height);
			}
		}
		if (m_showSceneViewport) {
			if (!(m_activeScene->getData().shaderFlags & ST_ShaderViewFlags_DisplayDepth))
				Gizmo::ApplyToFrameBuffer(m_sceneCamera.getCamera()->getFrameBuffer());
			m_sceneViewport.draw(m_sceneCamera, &m_showSceneViewport);
		}
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
		if (m_showBuildWindow) {
			drawBuildWindow();
		}

		Application::get().getImGuiLayer()->blockEvents(!(m_gameViewport.focused || m_sceneViewport.hovered || m_sceneViewport.focused) || Gizmo::IsUsing());
		StuluBindings::Input::s_enabled = m_gameViewport.focused;

		m_editorScene->updateAssemblyScripts("onDrawEditorGUI()");
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
				if (ImGui::MenuItem("Build Project")) {
					m_showBuildWindow = true;
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
				if (getEditorScene()->s_physics.isPVDRunning()) {
					if (ImGui::MenuItem("Stop PhysX Visual Debugger")) {
						getEditorScene()->s_physics.stopPVD();
					}
				}
				else {
					if (ImGui::MenuItem("Start PhysX Visual Debugger")) {
						getEditorScene()->s_physics.startPVD();
					}
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
				if (ImGui::MenuItem("Licenses/3rd Party/Dependencies")) {
					m_showLicensesWindow = true;
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
			//if (ImGui::ImageButton(reinterpret_cast<void*>((uint64_t)tex->getRendererID()), { size, size }, { 0, 1 }, { 1, 0 }, 0, { 0,0,0,0 }, icoColor)) {
			if (ImGui::ImageButton(tex, { size, size }, { 0, 1 }, { 1, 0 }, 0, { 0,0,0,0 }, icoColor)) {
				if (s_runtime) {
					onRuntimeStop();
				}
				else {
					onRuntimeStart();
				}
			}
		}ImGui::SameLine();
		{
			bool paused = Time::Scale == 0.0f;
			if (paused)
				icoColor = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size) - (size));
			Ref<Texture> tex = EditorResources::getPauseTexture();
			//if (ImGui::ImageButton(reinterpret_cast<void*>((uint64_t)tex->getRendererID()), { size, size }, { 0, 1 }, { 1, 0 }, 0, ImVec4(0,0,0,0), icoColor)) {
			if (ImGui::ImageButton(tex, { size, size }, { 0, 1 }, { 1, 0 }, 0, ImVec4(0,0,0,0), icoColor)) {
				if (paused) {
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
	void EditorLayer::drawBuildWindow() {
		if (ImGui::Begin("Build", &m_showBuildWindow)) {
			ComponentsRender::drawStringControl("Name", m_buildData.name);
			ComponentsRender::drawInt3Control("Version", (int&)m_buildData.version.major, (int&)m_buildData.version.minor, (int&)m_buildData.version.patch);
			ComponentsRender::drawUIntControl("Width", m_buildData.width);
			ComponentsRender::drawUIntControl("Height", m_buildData.height);
			ImGui::Separator();
			if (ImGui::TreeNodeEx("Scenes", ImGuiTreeNodeFlags_DefaultOpen)) {
				for (auto& sceneUUID : m_buildData.scenesToBuild) {
					Asset& asset = AssetsManager::get(sceneUUID);
					if (ImGui::RadioButton(asset.path.c_str(), m_buildData.startScene == sceneUUID)) {
						m_buildData.startScene = sceneUUID;
					}
				}
				ImVec4 icoColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
				ImVec4 bgColor = { 0,0,0,0 };
				//ImGui::Image(reinterpret_cast<void*>((uint64_t)EditorResources::getSceneTexture()->getRendererID()), ImVec2(30, 30), { 0, 1 }, { 1, 0 }, icoColor, bgColor);
				ImGui::Image(EditorResources::getSceneTexture(), ImVec2(30, 30), { 0, 1 }, { 1, 0 }, icoColor, bgColor);
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Scene)).c_str())) {
						UUID id = *(UUID*)payload->Data;
						if (AssetsManager::existsAndType(id, AssetType::Scene)) {
							m_buildData.scenesToBuild.push_back(id);
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::Text("Drag Scene here to add");
				ImGui::TreePop();
			}
			ImGui::Separator();
			ComponentsRender::drawStringControl("Path", m_buildData.path);
			if (ImGui::Button("Select Path"))
				m_buildData.path = Platform::browseFolder();

			bool disabled = !(std::filesystem::exists(m_buildData.path) && m_buildData.startScene != UUID::null);
			if (disabled) {
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			ImGui::SameLine();
			if (ImGui::Button("Build"))
				buildProject(m_buildData.path);
			if (disabled)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			ImGui::End();
		}
	}
#pragma endregion
#pragma region Scene
	void EditorLayer::onUpdate(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		if (!Gizmo::IsUsing()) {
			Input::s_enabled = m_sceneViewport.hovered || m_sceneViewport.focused;
			m_sceneCamera.updateMove(timestep);
			m_sceneCamera.onUpdate(timestep);
			m_activeScene->updateTransform(m_sceneCamera.getTransform());
			Input::s_enabled = true;
		}

		if (s_runtime) {
			m_activeScene->onUpdateRuntime(timestep);
			SceneRenderer::GenSceneTexture(m_sceneFrameBuffer, m_runtimeScene);
		}

		m_activeScene->onUpdateEditor(timestep, m_sceneCamera);

		drawObjectOutlines();
	}
	void EditorLayer::onRuntimeStart() {
		Time::Scale = 1.0f;
		s_runtime = true;
		m_runtimeScene = Scene::copy(m_editorScene);
		m_activeScene = m_runtimeScene;

		m_activeScene->onRuntimeStart();

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
	void EditorLayer::SaveScene() {
		ST_PROFILING_FUNCTION();
		std::string path = Platform::saveFile("Scene File (*.scene)\0 *.scene\0Stulu Scene File (*.ssc)\0 *.ssc\0All Files (*.*)\0*.*\0", EditorApp::getProject().path.c_str());
		if (!path.empty())
			SaveScene(path);
	}
	void EditorLayer::OpenScene() {
		ST_PROFILING_FUNCTION();
		if (s_runtime) {
			m_activeScene->onRuntimeStop();
			s_runtime = false;
		}
	
		std::string path = Platform::openFile(
			"Scene Files (*.scene)\0 *.scene\0Stulu Scene Files (*.ssc)\0 *.ssc\0All Files (*.*)\0*.*\0"
			, EditorApp::getProject().path.c_str());
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
			m_activeScene->onViewportResize(m_gameViewport.width, m_gameViewport.height);
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
		m_activeScene->onViewportResize(m_gameViewport.width, m_gameViewport.width);
		m_editorHierarchy.setScene(m_activeScene);
		DiscordRPC::setState("Editing a scene");
		ST_TRACE("New Scene loaded");
	}
#pragma endregion
#pragma region Gizmos
	void EditorLayer::drawObjectOutlines() {
		GameObject selected = m_editorHierarchy.getCurrentObject();
		if (selected) {
			auto& tc = selected.getComponent<TransformComponent>();
			m_sceneCamera.getCamera()->getFrameBuffer()->bind();
			Renderer::begin(m_sceneCamera.getCamera()->getProjectionMatrix(), glm::inverse(m_sceneCamera.getTransform().transform), glm::vec3(.0f), glm::vec3(.0f));
			RenderCommand::setDepthTesting(false);
			//draw object to stencil buffer with 0x1
			{
				RenderCommand::setStencil(StencilMode::WriteToBuffer);
				MeshFilterComponent meshFilter;
				if (selected.hasComponent<MeshRendererComponent>() && selected.saveGetComponent<MeshFilterComponent>(meshFilter)) {
					if (meshFilter.mesh.hasMesh) {
						Renderer::submit(meshFilter.mesh.mesh->getVertexArray(), EditorResources::getTransparentShader(), tc.transform);
					}
				}
				m_activeScene->m_registry.view<MeshFilterComponent, TransformComponent, MeshRendererComponent>().each([=](
					entt::entity go, MeshFilterComponent& meshFilter, TransformComponent& transform, MeshRendererComponent& meshRenderer
					) {
						if (transform.parent == selected) {
							GameObject object = GameObject{ go , m_activeScene.get() };
							if (meshFilter.mesh.hasMesh) {
								Renderer::submit(meshFilter.mesh.mesh->getVertexArray(), EditorResources::getTransparentShader(), tc.transform);
							}
						}
					});
				RenderCommand::setStencil(StencilMode::DisableWriting);
			}
			//draw only where stencil not buffer equals 
			RenderCommand::setCullMode(CullMode::BackAndFront);
			{
				RenderCommand::setStencil(StencilMode::BeginDrawFromBuffer);
				static auto getScaleAdd = [=](const TransformComponent& tc) -> glm::vec3 {
					float scaleAdd = .02f;
					return tc.worldScale + (scaleAdd * tc.worldScale);
				};

				MeshFilterComponent meshFilter;
				if (selected.hasComponent<MeshRendererComponent>() && selected.saveGetComponent<MeshFilterComponent>(meshFilter)) {
					if (meshFilter.mesh.hasMesh) {
						Renderer::submit(meshFilter.mesh.mesh->getVertexArray(), EditorResources::getOutlineShader(), Math::createMat4(tc.worldPosition, tc.worldRotation, getScaleAdd(tc)));

					}
				}

				//each child ouline
				m_activeScene->m_registry.view<MeshFilterComponent, TransformComponent, MeshRendererComponent>().each([=](
					entt::entity go, MeshFilterComponent& meshFilter, TransformComponent& transform, MeshRendererComponent& meshRenderer
					) {
						if (transform.parent == selected) {
							GameObject object = GameObject{ go , m_activeScene.get() };
							if (meshFilter.mesh.hasMesh) {
								Renderer::submit(meshFilter.mesh.mesh->getVertexArray(), EditorResources::getOutlineShader(), Math::createMat4(transform.worldPosition, transform.worldRotation, getScaleAdd(transform)));
							}
						}
					});

				RenderCommand::setStencil(StencilMode::EndDrawFromBuffer);
			}
			Renderer::end();
			m_sceneCamera.getCamera()->getFrameBuffer()->unbind();
		}
	}
	void EditorLayer::onRenderGizmo() {
		ST_PROFILING_FUNCTION();
		if (m_showSceneViewport) {
			if (ImGui::Begin("Scene", &m_showSceneViewport)) {
				Gizmo::Begin();
				const glm::vec3& cameraPos = m_sceneCamera.getTransform().worldPosition;
				const float gizmoViewDistance = 50.0f;
				//draw all cameras
				for (entt::entity goID : m_activeScene->getAllGameObjectsWith<CameraComponent>()) {
					GameObject go = GameObject(goID, m_activeScene.get());
					TransformComponent& transf = go.getComponent<TransformComponent>();
					float camDistance = glm::distance(cameraPos, transf.worldPosition);

					if(camDistance <= gizmoViewDistance)
						Renderer2D::drawTexturedQuad(Math::createMat4(transf.worldPosition, transf.worldRotation, 
							glm::vec3(.75f)) * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, -90, 0)))), EditorResources::getCameraTexture());
				}
				//draw all lights
				for (entt::entity goID : m_activeScene->getAllGameObjectsWith<LightComponent>()) {
					GameObject go = GameObject(goID, m_activeScene.get());
					TransformComponent& transf = go.getComponent<TransformComponent>();
					LightComponent& light = go.getComponent<LightComponent>();
					float camDistance = glm::distance(cameraPos, transf.worldPosition);

					if (camDistance <= gizmoViewDistance) {
						switch (light.lightType)
						{
						case LightComponent::Area:
							Gizmo::drawTextureBillBoard(EditorResources::getLightTexture(), transf.worldPosition, glm::vec3(.75f), glm::vec3(0, 1, 0), glm::vec4(light.color, 1.0f));
							break;
						case LightComponent::Directional:
							Renderer2D::drawTexturedQuad(
								Math::createMat4(transf.worldPosition, transf.worldRotation, glm::vec3(1.5f,.75f,1.5f)) * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, -90, 0)))),
								EditorResources::getDirectionalLightTexture(), glm::vec2(1.0f), glm::vec4(light.color, 1.0f));
							break;
						case LightComponent::Spot:
							Renderer2D::drawTexturedQuad(
								Math::createMat4(transf.worldPosition, transf.worldRotation, glm::vec3(.75f)) * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, -90, 0)))),
								EditorResources::getSpotLightTexture(), glm::vec2(1.0f), glm::vec4(light.color, 1.0f));
							break;
						}
					}
				}

				if (m_editorHierarchy.getCurrentObject()) {
					GameObject selected = m_editorHierarchy.getCurrentObject();
					onDrawGizmoSelected(selected);
				}
				m_activeScene->updateAssemblyScripts("onDrawGizmos()");
				Gizmo::End();
				
			}
			ImGui::End();
		}
	}
	void EditorLayer::onDrawGizmoSelected(GameObject selected) {
		auto& tc = selected.getComponent<TransformComponent>();
		{
			float snapValue = .0f;
			if (Input::isKeyDown(Keyboard::LeftControl)) {
				snapValue = 0.5f;
				if (m_gizmoEditType == GizmoTransformEditMode::Rotate)
					snapValue = 45.0f;
			}
			if (Gizmo::TransformEdit(tc, m_gizmoEditType, glm::vec3(snapValue))) {
				if (getEditorLayer().isRuntime() && getEditorScene()->getData().enablePhsyics3D) {
					getEditorScene()->updateTransformAndChangePhysicsPositionAndDoTheSameWithAllChilds(selected);
				}
			}
		}
		
		
		//camera view frustum
		if (selected.hasComponent<CameraComponent>()) {
			CameraComponent camera = selected.getComponent<CameraComponent>();
			float zNear = camera.settings.zNear;
			float zFar = camera.settings.zFar;
			float aspect = camera.settings.aspectRatio;
			glm::mat4 nearTransform;
			glm::mat4 farTransform;

			if (camera.mode == CameraMode::Perspective) {
				float fovHalf = glm::radians(camera.settings.fov*0.5f);
				float nearHeight = 1.0f;
				float nearWidth = nearHeight * aspect;
				float farHeight = 2.0f * zFar * (glm::tan(fovHalf));//because tan(fov) = ankhatet / gegenkahtet irgendwas mathe 9te klasse
				float farWidth = farHeight * aspect;
				glm::vec3 nearPos = tc.worldPosition + tc.forward * zNear;
				glm::vec3 farPos = tc.worldPosition + tc.forward * zFar;

				nearTransform = Math::createMat4(nearPos, tc.worldRotation, glm::vec3(nearWidth, nearHeight, 1.0f));
				farTransform = Math::createMat4(farPos, tc.worldRotation, glm::vec3(farWidth, farHeight, 1.0f));
			}
			else {
				float zoom = camera.settings.zoom;
				float height = 1.0f * zoom;;
				float width = height * aspect;
				glm::vec3 nearPos = tc.worldPosition + tc.forward * zNear;
				glm::vec3 farPos = tc.worldPosition + tc.forward * zFar;

				nearTransform = Math::createMat4(nearPos, tc.worldRotation, glm::vec3(width, height, 1.0f));
				farTransform = Math::createMat4(farPos, tc.worldRotation, glm::vec3(width, height, 1.0f));
			}

			Gizmo::drawRect(nearTransform, COLOR_WHITE_VEC4);
			Gizmo::drawRect(farTransform, COLOR_WHITE_VEC4);
			static glm::vec4 edgesPos[4]{
				{ -0.5f, -0.5f, 0.0f, 1.0f },
				{ 0.5f, -0.5f, 0.0f, 1.0f },
				{ 0.5f,  0.5f, 0.0f, 1.0f },
				{ -0.5f,  0.5f, 0.0f, 1.0f },
			};
			for (size_t i = 0; i < 4; i++) {
				Gizmo::drawLine(nearTransform * edgesPos[i], farTransform * edgesPos[i]);
			}
		}
		if (selected.hasComponent<LightComponent>()) {
			LightComponent light = selected.getComponent<LightComponent>();

			const glm::vec3& position = tc.worldPosition;
			glm::vec3 scale = glm::vec3(1.0f);

			if (light.lightType == LightComponent::Area) {
				scale = glm::vec3(light.areaRadius * 2.0f);
				Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
				Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(90.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
				Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, 90.0f, .0f))), scale), COLOR_GREEN, .02f);
			}
			else if (light.lightType == LightComponent::Spot) {
				
			}
			
		}
		//colliders
		BoxColliderComponent boxCollider;
		if (selected.saveGetComponent<BoxColliderComponent>(boxCollider)) {
			glm::vec3 position = tc.worldPosition + boxCollider.offset;
			glm::vec3 scale = tc.worldScale * (boxCollider.size * 2.0f);
			glm::mat4 transform = Math::createMat4(position, tc.worldRotation, scale);

			Gizmo::drawOutlineCube(transform, COLOR_GREEN);
		}
		SphereColliderComponent sphereCollider;
		if (selected.saveGetComponent<SphereColliderComponent>(sphereCollider)) {
			glm::vec3 position = tc.worldPosition + sphereCollider.offset;
			glm::vec3 scale = tc.worldScale * (sphereCollider.radius * 2.0f);
			
			Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
			Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(90.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
			Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, 90.0f, .0f))), scale), COLOR_GREEN, .02f);
		}
		RenderCommand::setWireFrame(true);
		CapsuleColliderComponent capsuleCollider;
		if (selected.saveGetComponent<CapsuleColliderComponent>(capsuleCollider)) {
			glm::vec3 position = tc.worldPosition + capsuleCollider.offset;
			glm::vec3 scale;
			if (capsuleCollider.horizontal)
				scale = tc.worldScale * (glm::vec3(capsuleCollider.radius, capsuleCollider.height / 2.0f, capsuleCollider.radius));
			else
				scale = tc.worldScale * (glm::vec3(capsuleCollider.height / 2.0f, capsuleCollider.radius, capsuleCollider.radius));

			Renderer::submit(Resources::getCapsuleMeshAsset().mesh->getVertexArray(),
				EditorResources::getGreenColorShader(),
				Math::createMat4(position, tc.worldRotation, scale));
		}
		MeshColliderComponent meshCollider;
		if (selected.saveGetComponent<MeshColliderComponent>(meshCollider)) {
			glm::vec3 position = tc.worldPosition;
			glm::vec3 scale = tc.worldScale;

			if (meshCollider.mesh.hasMesh) {
				if (meshCollider.convex && meshCollider.convexMesh)
					Renderer::submit(meshCollider.convexMesh->getVertexArray(),
						EditorResources::getGreenColorShader(),
						Math::createMat4(position, tc.worldRotation, scale));
				else
					Renderer::submit(meshCollider.mesh.mesh->getVertexArray(),
						EditorResources::getGreenColorShader(),
						Math::createMat4(position, tc.worldRotation, scale));
			}
		}
		RenderCommand::setWireFrame(false);
	}
#pragma endregion
#pragma region Events
	void EditorLayer::onAttach() {
		ST_PROFILING_FUNCTION();
		newScene();
		loadPanelConfig();
		DiscordRPC::setDetails("Idling in " + getEditorProject().name);
	}
	void EditorLayer::onEvent(Event& e) {
		ST_PROFILING_FUNCTION();
		if(m_sceneViewport.focused && m_sceneViewport.hovered)
			m_sceneCamera.onEvent(e);
		if (e.getEventType() == EventType::WindowResize) {
			m_sceneCamera.onResize((float)static_cast<WindowResizeEvent&>(e).getWidth(), (float)static_cast<WindowResizeEvent&>(e).getHeight());
		}
		
		m_activeScene->onEvent(e);

		EventDispatcher dispacther(e);
		dispacther.dispatch<KeyDownEvent>(ST_BIND_EVENT_FN(EditorLayer::onShortCut));
		dispacther.dispatch<WindowCloseEvent>(ST_BIND_EVENT_FN(EditorLayer::onApplicationQuit));
		//dispacther.dispatch<MouseButtonDownEvent>(ST_BIND_EVENT_FN(EditorLayer::onGameObjectPick));
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
			case Keyboard::U:
				m_gizmoEditType = GizmoTransformEditMode::Universal;
				DiscordRPC::setDetails("Using Universal mode in " + getEditorProject().name);
				break;
			case Keyboard::S:
				if (control) {
					if (shift)
						SaveScene();
					else
						SaveScene(m_currentScenePath);
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
#define ST_GizmoTransformEditMode_ONE_AXIS_IMPL_SHORTCUT(key) \
			case Keyboard::##key: \
				if (m_gizmoEditType & GizmoTransformEditMode::Translate_X || m_gizmoEditType & GizmoTransformEditMode::Translate_Y || m_gizmoEditType & GizmoTransformEditMode::Translate_Z) { \
					m_gizmoEditType = GizmoTransformEditMode::Translate_##key; \
				}\
				if (m_gizmoEditType & GizmoTransformEditMode::Rotate_X || m_gizmoEditType & GizmoTransformEditMode::Rotate_Y || m_gizmoEditType & GizmoTransformEditMode::Rotate_Z) { \
					m_gizmoEditType = GizmoTransformEditMode::Rotate_##key; \
				} \
				if (m_gizmoEditType & GizmoTransformEditMode::Scale_X || m_gizmoEditType & GizmoTransformEditMode::Scale_Y || m_gizmoEditType & GizmoTransformEditMode::Scale_Z) { \
					m_gizmoEditType = GizmoTransformEditMode::Scale_##key; \
				} \
				break;
				ST_GizmoTransformEditMode_ONE_AXIS_IMPL_SHORTCUT(X);
				ST_GizmoTransformEditMode_ONE_AXIS_IMPL_SHORTCUT(Y);
				ST_GizmoTransformEditMode_ONE_AXIS_IMPL_SHORTCUT(Z);
		}
		return false;
	}
	bool EditorLayer::onApplicationQuit(WindowCloseEvent& e) {
		DiscordRPC::shutdown();
		if (s_runtime)
			m_activeScene->onRuntimeStop();
		return false;
	}
	bool EditorLayer::onGameObjectPick(MouseButtonDownEvent& e) {
		glm::vec2 screen, screenSize;
		{
			glm::vec2 globalMouse = Input::getMousePos();
			glm::vec2 start = { m_sceneViewport.startPos.x,m_sceneViewport.startPos.y };

			screenSize = { (float)m_sceneViewport.width, (float)m_sceneViewport.height };
			screen = globalMouse - start;

			if (!(e.getButton() == MOUSE_BUTTON_LEFT && m_sceneViewport.hovered)) {
				return false;
			}
		}
		glm::mat4 viewProjectionInverse = glm::inverse(m_sceneCamera.getCamera()->getProjectionMatrix() * glm::inverse(m_sceneCamera.getTransform().transform));
		float normScreenX = 2.0f * screen.x / screenSize.x - 1.0f;
		float normScreenY = 2.0f * screen.y / screenSize.y - 1.0f;
		glm::vec3 pos;
		{
			glm::vec4 ndc = glm::vec4(normScreenX, normScreenY, -1.0f, 1.0f);
			glm::vec4 worldPos = viewProjectionInverse * ndc;
			worldPos.w = 1.0f / worldPos.w;
			pos.x = worldPos.x * worldPos.w;
			pos.y = worldPos.y * worldPos.w;
			pos.z = worldPos.z * worldPos.w;
		}
		pos += normScreenX * m_sceneCamera.getTransform().right * m_sceneCamera.getAspectRatioX() * 2.0f;
		pos += -normScreenY * m_sceneCamera.getTransform().up * m_sceneCamera.getAspectRatioY() * 2.0f;

		VFC::setCamera(m_sceneCamera.getAspectRatio(),
			m_sceneCamera.getNear(), m_sceneCamera.getFar(),
			m_sceneCamera.getFov(), m_sceneCamera.getTransform());

		std::vector<GameObject> objects;
		for (auto& obID : m_activeScene->getAllGameObjectsWith<TransformComponent, MeshFilterComponent, MeshRendererComponent>()) {
			auto& meshFilter = m_activeScene->m_registry.get<MeshFilterComponent>(obID);
			if (!meshFilter.mesh.mesh || !meshFilter.mesh.mesh->getBoundingBox())
				continue;

			Ref<BoundingBox> bb = meshFilter.mesh.mesh->getBoundingBox();
			if (VFC::isInView(bb, m_activeScene->m_registry.get<TransformComponent>(obID))) {
				objects.push_back(GameObject(obID, m_activeScene.get()));
			}
		}
		for (float zAdd = m_sceneCamera.getNear(); zAdd < m_sceneCamera.getFar(); zAdd += 1.0f) {
			pos += m_sceneCamera.getTransform().forward;

			for (auto& gameObject : objects) {
				if (gameObject.getComponent<MeshFilterComponent>().mesh.mesh->getBoundingBox()->isRayIntersecting(pos, m_sceneCamera.getTransform().forward)) {
					m_editorHierarchy.setSelectedGameObject(gameObject);
					return true;
				}
			}
			
		}
		return false;
	}
#pragma endregion
#pragma region Config
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
		if (values.find("build.name") != values.end())
			m_buildData.name = values["build.name"];
		if (values.find("build.width") != values.end())
			m_buildData.width = stoi(values["build.width"]);
		if (values.find("build.height") != values.end())
			m_buildData.height = stoi(values["build.height"]);
		if (values.find("build.major") != values.end())
			m_buildData.version.major = stoi(values["build.major"]);
		if (values.find("build.minor") != values.end())
			m_buildData.version.minor = stoi(values["build.minor"]);
		if (values.find("build.patch") != values.end())
			m_buildData.version.patch = stoi(values["build.patch"]);

			
		
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

		stream << "build.name=" << m_buildData.name << "\n";
		stream << "build.width=" << m_buildData.width << "\n";
		stream << "build.height=" << m_buildData.height << "\n";
		stream << "build.major=" << m_buildData.version.major << "\n";
		stream << "build.minor=" << m_buildData.version.minor << "\n";
		stream << "build.patch=" << m_buildData.version.patch << "\n";


		stream.close();
		ST_TRACE("Saved Editor Panel config to {0}", file);
	}
#pragma endregion

	void EditorLayer::buildProject(const std::string& dir) {
		if (std::filesystem::exists(dir))
			std::filesystem::remove_all(dir);

		SaveScene(m_currentScenePath);

		rebuildAssembly();
		const std::string projectDataPath = dir + "/" + m_buildData.name + "-data";
		if (!std::filesystem::exists(dir)) {
			std::filesystem::create_directory(dir);
		}
		if (!std::filesystem::exists(projectDataPath)) {
			std::filesystem::create_directories(projectDataPath + "/Managed");
			std::filesystem::create_directories(projectDataPath + "/assets");
		}

		std::filesystem::copy(getEditorProject().dataPath + "/ProjectAssembly.dll", projectDataPath + "/Managed", std::filesystem::copy_options::overwrite_existing);

		copyAllFilesFromDir("build", dir);
		std::vector<std::string> AllScnenesToBuild;
		for (auto& id : m_buildData.scenesToBuild) {
			if (AssetsManager::existsAndType(id, AssetType::Scene))
				AllScnenesToBuild.push_back(AssetsManager::get(id).path);
		}

		auto& assets = SceneSerializer::getAllSceneAssets(AllScnenesToBuild);
		for (UUID uuid : assets) {
			if (AssetsManager::exists(uuid)) {
				Asset asset = AssetsManager::get(uuid);
				std::string path = projectDataPath + "/assets/" + std::filesystem::relative(asset.path, getEditorProject().assetPath).string();
				std::string parent = std::filesystem::path(path).parent_path().string();
				if (!std::filesystem::exists(parent))
					std::filesystem::create_directories(parent);

				std::filesystem::copy(asset.path, path, std::filesystem::copy_options::overwrite_existing);
				std::filesystem::copy(asset.path + ".meta", path +".meta", std::filesystem::copy_options::overwrite_existing);
			}
		}

		if (std::filesystem::exists(dir + "/build")) {
			std::filesystem::remove_all(dir + "/build");
		}
		//copy engine stuff
		copyAllFilesFromDir("data/PhysX", dir + "/data/PhysX");
		copyAllFilesFromDir("mono", dir + "/mono");
		copyAllFilesFromDir("Stulu", dir + "/Stulu");
		std::filesystem::create_directories(dir + "/data/Managed");
		std::filesystem::copy("data/Managed/Stulu.ScriptCore.dll", dir + "/data/Managed/Stulu.ScriptCore.dll", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy("mono-2.0-sgen.dll", dir + "/mono-2.0-sgen.dll", std::filesystem::copy_options::overwrite_existing);

		//generate app file
		std::string name(m_buildData.name);
		Version version(m_buildData.version);
		WindowProps window;
		window.title = name;
		window.width = m_buildData.width;
		window.height = m_buildData.height;
		window.VSync = false;

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "App Name" << YAML::Value << name;
		out << YAML::Key << "App Verison" << YAML::Value << (glm::vec3) * ((glm::uvec3*)&version);
		out << YAML::Key << "Window Title" << YAML::Value << window.title;
		out << YAML::Key << "Window Width" << YAML::Value << window.width;
		out << YAML::Key << "Window Height" << YAML::Value << window.height;
		out << YAML::Key << "Window VSync" << YAML::Value << window.VSync;
		out << YAML::Key << "Start Scene" << YAML::Value << (uint64_t)m_buildData.startScene;
		out << YAML::EndMap;


		FILE* file = fopen((dir + "/Stulu/app").c_str(), "w");
		fprintf(file, out.c_str());
		fclose(file);


#ifdef ST_PLATFORM_WINDOWS
		std::filesystem::rename(dir + "/Stulu Runtime.exe", dir + "/" + name + ".exe");
#endif // ST_PLATFORM_WINDOWS

		ST_INFO("Finished building {0} in {1}\a", name, dir)
	}
	void copyAllFilesFromDir(const std::string& from, const std::string& to) {
		if (!std::filesystem::exists(to)) {
			std::filesystem::create_directories(to);
		}
		std::filesystem::copy(from, to, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
		/*for (std::filesystem::directory_entry dir : std::filesystem::recursive_directory_iterator(from)) {
			auto relativeToPath = std::filesystem::relative(dir, from);
			if(!dir.is_directory())
				std::filesystem::copy(dir.path(), to + "/" + relativeToPath.string());
		}*/
	}
}