#include "EditorHierarchy.h"

#include <imgui/imgui.h>

namespace Stulu {
	void EditorHierarchyPanel::setScene(const Ref<Scene>& scene) {
		m_selectedObject = GameObject::null;
		m_scene = scene;
	}
	void EditorHierarchyPanel::render(bool* open) {
		ST_PROFILING_FUNCTION();
		if (ImGui::Begin("Hierarchy", open)) {
			if (m_scene == nullptr)
				return noScene();

			m_scene->m_registry.each([&](auto goID) {
				GameObject gameObject{ goID, m_scene.get() };
				drawObject(gameObject);
				});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_selectedObject = GameObject::null;

			if (ImGui::BeginPopupContextWindow("Create")) {
				if (ImGui::MenuItem("GameObject")) {
					m_scene->createGameObject();
				}
				if (m_selectedObject != GameObject::null && ImGui::MenuItem("Child")) {
					m_scene->createGameObject("Child").getComponent<TransformComponent>().parent = m_selectedObject;
				}
				ImGui::Separator();
				if (ImGui::BeginMenu("3D")) {
					if (ImGui::MenuItem("Cube")) {
						GameObject g = m_scene->createGameObject("Cube");
						g.addComponent<MeshFilterComponent>().mesh = Resources::getCubeMeshAsset();
						g.addComponent<BoxColliderComponent>();
					}
					if (ImGui::MenuItem("Plane")) {
						GameObject g = m_scene->createGameObject("Plane");
						g.addComponent<MeshFilterComponent>().mesh = Resources::getPlaneMeshAsset();
						g.addComponent<MeshColliderComponent>().mesh = Resources::getPlaneMeshAsset();
					}
					if (ImGui::MenuItem("Sphere")) {
						GameObject g = m_scene->createGameObject("Sphere");
						g.addComponent<MeshFilterComponent>().mesh = Resources::getSphereMeshAsset();
						g.addComponent<SphereColliderComponent>();
					}
					if (ImGui::MenuItem("Capsule")) {
						GameObject g = m_scene->createGameObject("Capsule");
						g.addComponent<MeshFilterComponent>().mesh = Resources::getCapsuleMeshAsset();
						g.addComponent<CapsuleColliderComponent>();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("2D")) {
					if (ImGui::MenuItem("SpriteRenderer"))
						m_scene->createGameObject("SpriteRenderer").addComponent<SpriteRendererComponent>();
					if (ImGui::MenuItem("CircleRenderer"))
						m_scene->createGameObject("CircleRenderer").addComponent<CircleRendererComponent>();
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Lights")) {
					if (ImGui::MenuItem("Directional Light"))
						m_scene->createGameObject("Directional Light").addComponent<LightComponent>(LightComponent::Directional);
					if(ImGui::MenuItem("Area Light"))
						m_scene->createGameObject("Area Light").addComponent<LightComponent>(LightComponent::Area);
					if (ImGui::MenuItem("Spot Light")) 
						m_scene->createGameObject("Spot Light").addComponent<LightComponent>(LightComponent::Spot);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Rendering")) {
					if (ImGui::MenuItem("Orthographic Camera")) {
						GameObject g = m_scene->createGameObject("Orthographic Camera");
						g.addComponent<CameraComponent>(CameraMode::Orthographic, true);
						g.getComponent<GameObjectBaseComponent>().tag = "MainCam";
					}
					if (ImGui::MenuItem("Perspective Camera")) {
						GameObject g = m_scene->createGameObject("Perspective Camera");
						g.addComponent<CameraComponent>(CameraMode::Perspective, true);
						g.getComponent<GameObjectBaseComponent>().tag = "MainCam";
						g.saveAddComponent<PostProcessingComponent>();
					}
					ImGui::EndMenu();
				}
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}
	void EditorHierarchyPanel::drawObject(GameObject gameObject, int childIndex) {
		auto transform = gameObject.getComponent<TransformComponent>();
		if (transform.parent && childIndex == 0)
			return;
		GameObjectBaseComponent baseComp = gameObject.getComponent<GameObjectBaseComponent>();
		ImGui::SetCursorPosX(10.0f * childIndex);
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (m_selectedObject == gameObject ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool open = ImGui::TreeNodeEx((void*)gameObject, flags, baseComp.name.c_str());
		if(ImGui::IsItemClicked())
			m_selectedObject = gameObject;

		//drag drop
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("GAMEOBJECT_HIERARCHY_MOVE", &gameObject, sizeof(GameObject));
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_HIERARCHY_MOVE")) {
				ST_ASSERT(payload->DataSize == sizeof(GameObject), "GAMEOBJECT_HIERARCHY_MOVE Drag Drop payload size wrong");
				GameObject go = *(const GameObject*)payload->Data;
				transform.addChild(go);
			}
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Material)).c_str())) {
				UUID id = *(const UUID*)payload->Data;
				if (AssetsManager::existsAndType(id, AssetType::Material)) {
					m_scene->m_registry.view<TransformComponent>().each([=](entt::entity go, TransformComponent& transform) {
						if (transform.parent == gameObject) {
							GameObject child(go, m_scene.get());
							if (child.hasComponent<MeshRendererComponent>()) {
								child.getComponent<MeshRendererComponent>().material = std::any_cast<Ref<Material>>(AssetsManager::get(id).data);
							}
						}
					});
					if (gameObject.hasComponent<MeshRendererComponent>())
						gameObject.getComponent<MeshRendererComponent>().material = std::any_cast<Ref<Material>>(AssetsManager::get(id).data);
				}
			}
			ImGui::EndDragDropTarget();
		}

		//end
		if (open) {
			m_scene->m_registry.view<TransformComponent>().each([=](entt::entity go, TransformComponent& transform) {
				if(transform.parent == gameObject)
					drawObject({ go, m_scene.get() }, childIndex + 1);
			});
			ImGui::TreePop();
		}
		ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x,1));
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_HIERARCHY_MOVE")) {
				ST_ASSERT(payload->DataSize == sizeof(GameObject), "GAMEOBJECT_HIERARCHY_MOVE Drag Drop payload size wrong");
				GameObject go = *(const GameObject*)payload->Data;
				if (childIndex == 0)
					go.getComponent<TransformComponent>().parent = GameObject::null;
				else
					transform.parent.getComponent<TransformComponent>().addChild(go);
			}
			ImGui::EndDragDropTarget();
		}
	}
	void EditorHierarchyPanel::noScene() {
		ImGui::Text("No Scene loaded");
		ImGui::End();
	}
}