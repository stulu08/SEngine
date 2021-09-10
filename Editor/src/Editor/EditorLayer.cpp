#include "EditorLayer.h"
#include <imgui/imgui.h>
#include "Editor/EditorApp.h"
namespace Stulu {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_cameraController(new CamerControllerSetting()) {
		RenderCommand::setClearColor(glm::vec4(0.15f));
		m_skybox = Skybox(CubeMap::create({
			"assets/SkyBox/city/right.jpg",
			"assets/SkyBox/city/left.jpg",
			"assets/SkyBox/city/top.jpg",
			"assets/SkyBox/city/bottom.jpg",
			"assets/SkyBox/city/front.jpg",
			"assets/SkyBox/city/back.jpg"
			}));
		car = Model("assets/car.obj");
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
	}
	int inspectorItem = -1;

	void EditorLayer::onUpdate(Timestep timestep) {
		m_cameraController.onUpdate(timestep);
		m_frameBuffer->bind();
		RenderCommand::clear();

		Renderer::beginScene(*m_cameraController.getCamera());
		Renderer2D::beginScene(*m_cameraController.getCamera());

		m_skybox.draw(*m_cameraController.getCamera());
		if (light != nullptr) {
			shaderLib.get("pbr")->bind();
			shaderLib.get("pbr")->setFloat3("u_lightColor", light->color);
			shaderLib.get("pbr")->setFloat3("u_lightPosition", light->transform.position);
			shaderLib.get("pbr")->setFloat("u_lightStrength", light->strength);
			shaderLib.get("pbr")->unbind();
		}
		for (auto object : objects) {
			if (!object->enabled)
				continue;
			for (auto component : object->components) {
				component->transform = object->transform;
				component->onUpdate(timestep);
			}
		}
		Renderer2D::endScene();
		Renderer::endScene();
		m_frameBuffer->unBind();
	}
	bool about = false;
	void EditorLayer::onImguiRender(Timestep timestep) {
		ImGui::DockSpaceOverViewport();
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New Scene")) {
					objects.clear();
					inspectorItem = -1;
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

			if (m_viewPortPanelWidth != viewportSize.x || m_viewPortPanelHeight != viewportSize.y) {
				m_viewPortPanelWidth = viewportSize.x;
				m_viewPortPanelHeight = viewportSize.y;
				m_frameBuffer->Resize(m_viewPortPanelWidth, m_viewPortPanelHeight);
				m_cameraController.onResize((float)m_viewPortPanelWidth, (float)m_viewPortPanelHeight);
			}
			ImGui::Image((void*)m_frameBuffer->getColorAttachmentRendereID(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::Begin("Explorer");


		ImGui::End();
		ImGui::Begin("Hirachy", (bool*)true, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Add")) {
				if (ImGui::MenuItem("Cube")) {
					GameObject* obje = new GameObject();
					obje->name = new std::string("Cube");
					obje->components.push_back(new ModelRenderer(cube, shaderLib.get("pbr")));
					obje->components.push_back(new PBRShaderEdit(shaderLib.get("pbr")));
					objects.push_back(obje);
				}
				if (ImGui::MenuItem("Car")) {
					GameObject* obje = new GameObject();
					obje->name = new std::string("car");
					obje->components.push_back(new ModelRenderer(car, shaderLib.get("pbr")));
					obje->components.push_back(new PBRShaderEdit(shaderLib.get("pbr")));
					objects.push_back(obje);
				}
				if (ImGui::MenuItem("Light")) {
					GameObject* obje = new GameObject();
					obje->name = new std::string("Light");
					light = new Light();
					obje->components.push_back(light);
					objects.push_back(obje);
				}
				if (ImGui::MenuItem("Sphere")) {
					GameObject* obje = new GameObject();
					obje->name = new std::string("Sphere");
					obje->components.push_back(new ModelRenderer(sphere, shaderLib.get("pbr")));
					obje->components.push_back(new PBRShaderEdit(shaderLib.get("pbr")));
					objects.push_back(obje);
				}
				if (ImGui::MenuItem("Reflection Sphere")) {
					GameObject* obje = new GameObject();
					obje->name = new std::string("Reflection Sphere");
					obje->components.push_back(new ModelRenderer(sphere, shaderLib.get("Reflective")));
					objects.push_back(obje);
				}
				if (ImGui::MenuItem("ParticleSystem")) {
					GameObject* obje = new GameObject();
					obje->name = new std::string("ParticleSystem");
					obje->components.push_back(new ParticleSystemComponent());
					objects.push_back(obje);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		for (int i = 0; i < objects.size(); i++) {
			if(!objects[i]->enabled)
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, .5f);
			else
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
			if (ImGui::Selectable(objects[i]->name->c_str(), inspectorItem == i))
				inspectorItem = i;
			ImGui::PopStyleVar();
		}

		ImGui::End();
		ImGui::Begin("Inspector");
		if (inspectorItem > -1) {
			if (ImGui::Button("Delete")) {
				objects.erase(objects.begin() + inspectorItem);
				inspectorItem = -1;
				ImGui::End();
				return;
			}
			imGui::checkBox("Enabled", objects[inspectorItem]->enabled, false); ImGui::SameLine();
			ImGui::InputText("Name", (char*)objects[inspectorItem]->name->c_str(), objects[inspectorItem]->name->capacity() + 1);
			if (ImGui::CollapsingHeader("Transform")) {
				objects[inspectorItem]->transform.drawImGui();
			}
			for (auto component : objects[inspectorItem]->components) {
				if (ImGui::CollapsingHeader(component->getTypeName())) {
					component->drawImGui();
					if (ImGui::Button("Remove Component")) {
						std::vector<Component*>::iterator it = std::find(objects[inspectorItem]->components.begin(), objects[inspectorItem]->components.end(), component);
						if (it != objects[inspectorItem]->components.end())
							objects[inspectorItem]->components.erase(it);

						break;
					}
				}
			}
			ImGui::Separator();

		}
		ImGui::End();
	}

	void EditorLayer::onEvent(Event& e) {
		m_cameraController.onEvent(e);
	}
}