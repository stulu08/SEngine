#include "EditorLayer.h"
#include "Editor/EditorApp.h"
#include "Editor/DiscordRPC.h"

#include "Stulu/ScriptCore/Bindings/Input.h"

#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>


namespace Stulu {
	void copyAllFilesFromDir(const std::string& from, const std::string& to);

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_sceneCamera(0.0, 85.0f, .001f, 1000.0f, 1) {
		ST_PROFILING_FUNCTION();

		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));

		FrameBufferSpecs fspecs;
		fspecs.width = Stulu::Application::get().getWidth();
		fspecs.height = Stulu::Application::get().getHeight();
		fspecs.colorTexture.format = TextureFormat::RGBA16F;
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

#pragma region GUI
	void EditorLayer::onImguiRender(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		ImGui::DockSpaceOverViewport();
		drawMenuBar();

		SceneData& data = m_activeScene->m_data;
		if (m_showDebugWindow) {
			if (ImGui::Begin("Debug Window", &m_showDebugWindow)) {
				if (ImGui::TreeNodeEx("Bloom")) {

					ComponentsRender::drawFloatControl("SampleScale", m_activeScene->getData().graphicsData.sampleScale);

					auto& data = m_sceneCamera.getPostProcessingData().bloomData;
					{//downsample
						static uint32_t sample = 0;
						static uint32_t width = 0, height = 0;
						ComponentsRender::drawUIntSliderControl(std::string("D index(") + std::to_string(width) + "x" + std::to_string(height) + ")", sample, 0, data.samples - 1);
						auto& texture = data.downSample[sample];
						if (texture) {
							width = texture->getWidth();
							height = texture->getHeight();
							ImGui::Image(texture, glm::vec2(200.0f) * m_sceneCamera.getAspectRatioXY(), { 0,1 }, { 1,0 }, glm::vec4(1.0f), glm::vec4(COLOR_GRAY_VEC4));
						}
					}
					{//upsample
						static uint32_t sample = 0;
						static uint32_t width = 0, height = 0;
						ComponentsRender::drawUIntSliderControl(std::string("U index(") + std::to_string(width) + "x" + std::to_string(height) + ")", sample, 0, data.samples - 1);
						auto& texture = data.upSample[sample];
						if (texture) {
							width = texture->getWidth();
							height = texture->getHeight();
							ImGui::Image(texture, glm::vec2(200.0f) * m_sceneCamera.getAspectRatioXY(), { 0,1 }, { 1,0 }, glm::vec4(1.0f), glm::vec4(COLOR_GRAY_VEC4));
						}
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNodeEx("Shadows")) {
					Ref<Texture> tex = m_activeScene->getRenderer()->m_shadowMap->getTexture()->getDepthAttachment();
					ImGui::Image(tex, glm::vec2(256.0f), {0,1}, {1, 0});
					ImGui::TreePop();
				}
				if (ImGui::TreeNodeEx("Display Only:")) {
					ComponentsRender::drawBoolBitFlagControl("Wireframe", ST_ShaderViewFlags_DisplayVertices, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Depth", ST_ShaderViewFlags_DisplayDepth, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Lighting", ST_ShaderViewFlags_DisplayLighting, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Normals", ST_ShaderViewFlags_DisplayNormal, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Tex Coords", ST_ShaderViewFlags_DisplayTexCoords, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Diffuse", ST_ShaderViewFlags_DisplayDiffuse, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Specular", ST_ShaderViewFlags_DisplaySpecular, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Roughness", ST_ShaderViewFlags_DisplayRoughness, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Metallic", ST_ShaderViewFlags_DisplayMetallic, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Ambient Occlusion", ST_ShaderViewFlags_DisplayAmbient, data.shaderFlags);
					ComponentsRender::drawBoolBitFlagControl("Emission", ST_ShaderViewFlags_DisplayEmission, data.shaderFlags);
					ImGui::TreePop();
				}
				if (ImGui::TreeNodeEx("Actions")) {
					static bool konvex = false;
					ImGui::Checkbox("Konvex", &konvex);
					if (ImGui::Button("Add Mesh Collider to child")) {
						GameObject gameObject = m_editorHierarchy.getCurrentObject();
						if (gameObject) {

							std::function<void(GameObject)> fn = [&](GameObject parent) {
								for (auto& goId : parent.getScene()->getAllGameObjectsWith<TransformComponent, MeshFilterComponent, MeshRendererComponent>()) {
									GameObject go = GameObject(goId, parent.getScene());
									if (go.getComponent<TransformComponent>().parent == parent) {
										go.saveAddComponent<MeshColliderComponent>().mesh = go.getComponent<MeshFilterComponent>().mesh;
										go.saveAddComponent<MeshColliderComponent>().convex = konvex;
										fn(go);
									}
								}
							};

							fn(gameObject);
						}
					}
					if (ImGui::Button("Add Box Collider to child")) {
						GameObject gameObject = m_editorHierarchy.getCurrentObject();
						if (gameObject) {

							std::function<void(GameObject)> fn = [&](GameObject parent) {
								for (auto& goId : parent.getScene()->getAllGameObjectsWith<TransformComponent, MeshFilterComponent, MeshRendererComponent>()) {
									GameObject go = GameObject(goId, parent.getScene());
									if (go.getComponent<TransformComponent>().parent == parent) {
										go.saveRemoveComponent<MeshColliderComponent>();
										Ref<BoundingBox> bb = go.getComponent<MeshFilterComponent>().mesh.mesh->getBoundingBox();
										auto& tc = go.getComponent<TransformComponent>();
										if (bb && bb->getType() == BoundingBoxType::AABB) {
											Ref<BoundingBoxAABB> aabb = std::dynamic_pointer_cast<BoundingBoxAABB>(bb);
											go.saveAddComponent<BoxColliderComponent>().offset = aabb->getCenter() * tc.worldScale;
											go.saveAddComponent<BoxColliderComponent>().size = aabb->getExtents();
										}
										else {
											go.saveAddComponent<BoxColliderComponent>();
										}

										fn(go);
									}
								}
							};

							fn(gameObject);
						}
					}
					if (ImGui::Button("Generate 10x10 Cube Layer")) {
						GameObject result = m_activeScene->createGameObject("Result");
						for (int i = 0; i < 10; i++) {
							for (int j = 0; j < 10; j++) {
								GameObject obj = m_activeScene->createGameObject(std::to_string(i) + "_" + std::to_string(j));
								result.getComponent<TransformComponent>().addChild(obj);
								obj.saveAddComponent<TransformComponent>().position = { i,.0f,j };
								obj.saveAddComponent<MeshFilterComponent>(Resources::getCubeMeshAsset());
								obj.saveAddComponent<MeshColliderComponent>().mesh = Resources::getCubeMeshAsset();
								obj.saveAddComponent<MeshColliderComponent>().convex = konvex;
								obj.saveAddComponent<RigidbodyComponent>();
							}
						}
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNodeEx("Shaders")) {
					for (UUID& id : AssetsManager::getAllByType(AssetType::Shader)) {
						if (id == UUID::null)
							continue;
						Asset& asset = AssetsManager::get(id);
						Ref<Shader>& shader = AssetsManager::getAs<Ref<Shader>>(id);
						if (ImGui::TreeNodeEx(shader->getName().c_str())) {
							if (ImGui::TreeNodeEx("Source")) {
								static bool preProcessed = false;
								ImGui::Checkbox("Pre Processed", &preProcessed);
								ImGui::TextUnformatted(shader->getSource(preProcessed).c_str());
								ImGui::TreePop();
							}
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}
			ImGui::End();
		}

		//Scene Settings
		{
			static bool useMainCam = true;
			if (m_showSceneSettingsPanel) {
				if (ImGui::Begin("Scene Settings", &m_showSceneSettingsPanel)) {
					if (ImGui::TreeNodeEx("Graphics")) {
						if (ImGui::TreeNodeEx("Scene Camera Settings")) {
							ComponentsRender::drawBoolControl("Use main camera settings", useMainCam);
							if (useMainCam) {
								ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
								ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
							}
							PostProcessingComponent comp;
							comp.data = m_sceneCamera.getPostProcessingData();
							ComponentsRender::drawComponent(GameObject::null, comp);
							m_sceneCamera.getPostProcessingData() = comp.data;

							if (useMainCam) {
								ImGui::PopItemFlag();
								ImGui::PopStyleVar();
							}
							ImGui::TreePop();
						}
						if (ImGui::TreeNodeEx("Scene")) {
							auto& gData = m_activeScene->m_data.graphicsData;
							if (ImGui::TreeNodeEx("Shadows")) {
								ComponentsRender::drawFloatControl("Distance", gData.shadowDistance);
								{
									enum class Quality : uint32_t {
										Custom = 0,
										Low = 1,
										Medium = 2,
										High = 3,
										Ultra = 4
									} quality;

									if (gData.shadowMapSize == 1024) {
										quality = Quality::Low;
									}
									else if (gData.shadowMapSize == 2048) {
										quality = Quality::Medium;
									}
									else if (gData.shadowMapSize == 4096) {
										quality = Quality::High;
									}
									else if (gData.shadowMapSize == 8192) {
										quality = Quality::Ultra;
									}
									else {
										quality = Quality::Custom;
									}


									if (ComponentsRender::drawEnumComboControl("Quality", quality)) {

										switch (quality)
										{
										case Quality::Low:
											gData.shadowMapSize = 1024;
											gData.shadowFar = 100.0f;
											break;
										case Quality::Medium:
											gData.shadowMapSize = 2048;
											gData.shadowFar = 250.0f;
											break;
										case Quality::High:
											gData.shadowMapSize = 4096;
											gData.shadowFar = 350.0f;
											break;
										case Quality::Ultra:
											gData.shadowMapSize = 8192;
											gData.shadowFar = 500.0f;
											break;
										}
										m_activeScene->getRenderer()->resizeShadowMap();
									}
								}
								ComponentsRender::drawFloatControl("Far", gData.shadowFar);
								if (ComponentsRender::drawUIntControl("Map Size", gData.shadowMapSize)) {
									m_activeScene->getRenderer()->resizeShadowMap();
								}

								
							}
							ComponentsRender::drawFloatSliderControl("Environment Lod", gData.env_lod, 0.0f, 4.0f);
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
			m_sceneCamera.setPostProcessingUsingMain(useMainCam);
		}

		if (m_showGameViewport) {
			m_gameViewport.draw(m_sceneFrameBuffer, &m_showGameViewport);
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
			m_inspectorPanel.render(&m_showInspector);
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
				if (PhysX::isPVDRunning()) {
					if (ImGui::MenuItem("Stop PhysX Visual Debugger")) {
						PhysX::stopPVD();
					}
				}
				else {
					if (ImGui::MenuItem("Start PhysX Visual Debugger")) {
						if (!PhysX::started()) {
							PhysX::startUp();
						}
						PhysX::startPVD();
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
#ifndef ST_DIST
				if (ImGui::MenuItem("Debug Window", (const char*)0, m_showDebugWindow)) {
					m_showDebugWindow = !m_showDebugWindow;
				}
#endif // !ST_DIST
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
		ST_PROFILING_FUNCTION();
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
		ST_PROFILING_FUNCTION();
		if (ImGui::Begin("Build", &m_showBuildWindow)) {
			ComponentsRender::drawStringControl("Name", m_buildData.name);
			ComponentsRender::drawInt3Control("Version", (int&)m_buildData.version.major, (int&)m_buildData.version.minor, (int&)m_buildData.version.patch);
			ComponentsRender::drawUIntControl("Width", m_buildData.width);
			ComponentsRender::drawUIntControl("Height", m_buildData.height);
			ComponentsRender::drawBoolControl("Debug", m_buildData.debug); ImGui::SameLine();
			ComponentsRender::drawHelpMarker("Currently only enables debug fps logging");
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
			if (ImGui::Button("Build")) {
				buildProject(m_buildData.path + "/" + m_buildData.name);
			}
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
			m_activeScene->onUpdateRuntime(timestep, m_gameViewport.drawn);
			if (m_gameViewport.drawn) {
				m_runtimeScene->getRenderer()->GenSceneTexture(m_sceneFrameBuffer);
			}
		}
		else if (m_gameViewport.drawn) {
			GameObject mainCamObj = m_activeScene->getMainCamera();
			if (m_activeScene->getMainCamera() && mainCamObj.hasComponent<CameraComponent>()) {
				CameraComponent& cam = mainCamObj.getComponent<CameraComponent>();
				m_activeScene->setupSceneForRendering(false);
				m_activeScene->renderSceneForCamera(mainCamObj, false);
				m_activeScene->closeSceneForRendering();
				m_sceneFrameBuffer->bind();
				RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, .0f));
				RenderCommand::clear();
				m_sceneFrameBuffer->unbind();
				if (!cam.settings.isRenderTarget) {
					if (cam.postProcessing)
						m_activeScene->getRenderer()->ApplyPostProcessing(m_sceneFrameBuffer, cam.getFrameBuffer()->getTexture(), *cam.postProcessing);
					else
						m_activeScene->getRenderer()->ApplyPostProcessing(m_sceneFrameBuffer, cam.getFrameBuffer()->getTexture());
				}
			}
		}
		else {
			m_editorScene->clearAllParticles();
		}

		m_activeScene->onUpdateEditor(timestep, m_sceneCamera, m_sceneViewport.drawn);

		drawObjectOutlines();
	}
	void EditorLayer::onRuntimeStart() {
		ST_PROFILING_FUNCTION();
		Time::Scale = 1.0f;
		s_runtime = true;
		m_runtimeScene = Scene::copy(m_editorScene);
		m_activeScene = m_runtimeScene;

		m_activeScene->onRuntimeStart();

		GameObject selected = m_editorHierarchy.getCurrentObject();//in editor scene
		m_editorHierarchy.setScene(m_activeScene);
		if (selected)
			m_editorHierarchy.setSelectedGameObject(GameObject::getById(selected.getComponent<GameObjectBaseComponent>().getUUID(), m_activeScene.get()));
	}
	void EditorLayer::onRuntimeStop() {
		ST_PROFILING_FUNCTION();
		s_runtime = false;
		m_activeScene->onRuntimeStop();
		m_activeScene = m_editorScene;

		GameObject selected = m_editorHierarchy.getCurrentObject();//in runtime scene
		m_editorHierarchy.setScene(m_activeScene);
		if (selected)
			m_editorHierarchy.setSelectedGameObject(GameObject::getById(selected.getComponent<GameObjectBaseComponent>().getUUID(), m_activeScene.get()));

		m_runtimeScene = nullptr;
	}
	void EditorLayer::SaveScene() {
		std::string path = Platform::saveFile("Scene File (*.scene)\0 *.scene\0Stulu Scene File (*.ssc)\0 *.ssc\0All Files (*.*)\0*.*\0", EditorApp::getProject().path.c_str());
		if (!path.empty())
			SaveScene(path);
	}
	void EditorLayer::OpenScene() {
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
		ST_PROFILING_FUNCTION();
		GameObject selected = m_editorHierarchy.getCurrentObject();
		if (selected) {
			auto& tc = selected.getComponent<TransformComponent>();
			m_sceneCamera.getCamera()->getFrameBuffer()->bind();
			Renderer::uploadCameraBufferData(m_sceneCamera.getCamera()->getProjectionMatrix(), glm::inverse(m_sceneCamera.getTransform().transform), glm::vec3(.0f), glm::vec3(.0f));
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
				RenderCommand::setStencil(StencilMode::EndDrawFromBuffer);
			}
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
		ST_PROFILING_FUNCTION();
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
		else if (e.getEventType() == EventType::WindowResize) {
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
		ST_PROFILING_FUNCTION();
		DiscordRPC::shutdown();
		if (s_runtime)
			m_activeScene->onRuntimeStop();
		return false;
	}
	bool EditorLayer::onGameObjectPick(MouseButtonDownEvent& e) {
		ST_PROFILING_FUNCTION();
		return false;
	}
#pragma endregion
#pragma region Config
	void EditorLayer::loadPanelConfig() {
		ST_PROFILING_FUNCTION();
		{
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
#ifndef ST_DIST
			if (values.find("m_showDebugWindow") != values.end())
				m_showDebugWindow = std::stoi(values["m_showDebugWindow"]);
#endif
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
		}
		{
			std::string file = getEditorProject().configPath + "/editor-camera-config.ini";
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
			if (values.find("postProcessingData.exposure") != values.end())
				m_sceneCamera.getPostProcessingData().exposure = std::stof(values["postProcessingData.exposure"]);
			if (values.find("postProcessingData.gamma") != values.end())
				m_sceneCamera.getPostProcessingData().gamma = std::stof(values["postProcessingData.gamma"]);
			if (values.find("postProcessingData.bloomIntensity") != values.end())
				m_sceneCamera.getPostProcessingData().bloomData.bloomIntensity = std::stof(values["postProcessingData.bloomIntensity"]);
			if (values.find("postProcessingData.bloomTreshold") != values.end())
				m_sceneCamera.getPostProcessingData().bloomData.bloomTreshold = std::stof(values["postProcessingData.bloomTreshold"]);
			if (values.find("postProcessingData.bloomEnabled") != values.end())
				m_sceneCamera.getPostProcessingData().bloomData.bloomEnabled = std::stof(values["postProcessingData.bloomEnabled"]);
			m_sceneCamera.getPostProcessingData() = m_sceneCamera.getPostProcessingData();

			ST_TRACE("Loaded Editor Camera config from {0}", file);
		}
		return;
	}
	void EditorLayer::savePanelConfig() {
		ST_PROFILING_FUNCTION();
		{
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
			stream << "m_showDebugWindow=" << (int)m_showDebugWindow << "\n";

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
			ST_TRACE("Saved Editor Camera config to {0}", file);
		}
		{
			std::string file = getEditorProject().configPath + "/editor-camera-config.ini";
			std::remove(file.c_str());
			std::fstream stream(file, std::ios::out);

			stream << "postProcessingData.exposure=" << m_sceneCamera.getPostProcessingData().exposure << "\n";
			stream << "postProcessingData.gamma=" << m_sceneCamera.getPostProcessingData().gamma << "\n";
			stream << "postProcessingData.bloomIntensity=" << m_sceneCamera.getPostProcessingData().bloomData.bloomIntensity << "\n";
			stream << "postProcessingData.bloomTreshold=" << m_sceneCamera.getPostProcessingData().bloomData.bloomTreshold << "\n";
			stream << "postProcessingData.bloomEnabled=" << m_sceneCamera.getPostProcessingData().bloomData.bloomEnabled << "\n";

			stream.close();
			ST_TRACE("Saved Editor Camera config to {0}", file);
		}
		
	}
#pragma endregion

	void EditorLayer::buildProject(const std::string& dir) {
		ST_PROFILING_FUNCTION();
		if (std::filesystem::exists(dir))
			std::filesystem::remove_all(dir);

		SaveScene(m_currentScenePath);

		getEditorProject().rebuildAssembly();
		const std::string projectDataPath = dir + "/" + m_buildData.name + "-data";
		if (!std::filesystem::exists(dir)) {
			std::filesystem::create_directory(dir);
		}
		if (!std::filesystem::exists(projectDataPath)) {
			std::filesystem::create_directories(projectDataPath + "/Managed");
			std::filesystem::create_directories(projectDataPath + "/assets");
			std::filesystem::create_directories(projectDataPath + "/Licenses");
		}

		std::filesystem::copy(getEditorProject().dataPath + "/ProjectAssembly.dll", projectDataPath + "/Managed", std::filesystem::copy_options::overwrite_existing);

		copyAllFilesFromDir("Build", dir);
		std::vector<std::string> AllScnenesToBuild;
		for (auto& id : m_buildData.scenesToBuild) {
			if (AssetsManager::existsAndType(id, AssetType::Scene))
				AllScnenesToBuild.push_back(AssetsManager::get(id).path);
		}

		copyAllFilesFromDir(getEditorProject().assetPath, projectDataPath + "/assets/");

		if (std::filesystem::exists(dir + "/Build")) {
			std::filesystem::remove_all(dir + "/Build");
		}
		//copy engine stuff
		copyAllFilesFromDir("Data/PhysX", dir + "/Data/PhysX");
		copyAllFilesFromDir("mono", dir + "/mono");
		copyAllFilesFromDir("Stulu", dir + "/Stulu");
		copyAllFilesFromDir("Licenses", dir + "/Licenses");

		std::filesystem::create_directories(dir + "/Data/Managed");
		std::filesystem::copy("Data/Managed/Stulu.ScriptCore.dll", dir + "/Data/Managed/Stulu.ScriptCore.dll", std::filesystem::copy_options::overwrite_existing);
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
		out << YAML::Key << "debug" << YAML::Value << m_buildData.debug;
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
	}
}