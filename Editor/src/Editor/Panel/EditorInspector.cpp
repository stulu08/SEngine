#include "EditorInspector.h"
namespace Stulu {
	/// <summary>
	/// Returns false if gameobject was deleted
	/// </summary>
	/// <param name="gameObject"></param>
	/// <returns></returns>
	bool EditorInspectorPanel::render(GameObject gameObject, bool* open) {
		ST_PROFILING_FUNCTION();
		if (ImGui::Begin("Inspector", open)) {
			if (gameObject == GameObject::null) {
				ImGui::End();
				return true;
			}
			else if (!gameObject.hasComponent<GameObjectBaseComponent>()) {
				ImGui::End();
				return true;
			}
			auto base = gameObject.getComponent<GameObjectBaseComponent>();

			if (ImGui::CollapsingHeader(base.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

				ComponentsRender::drawComponent<GameObjectBaseComponent>(gameObject, base);
				drawComponent<TransformComponent>(gameObject, "Transform", false);
				drawComponent<CameraComponent>(gameObject, "Camera");
				drawComponent<SkyBoxComponent>(gameObject, "SkyBox");
				drawComponent<SpriteRendererComponent>(gameObject, "SpriteRenderer");
				drawComponent<LightComponent>(gameObject, "Light");
				drawComponent<MeshRendererComponent>(gameObject, "MeshRenderer");
				drawComponent<MeshFilterComponent>(gameObject, "MeshFilter");
				drawComponent<RigidbodyComponent>(gameObject, "Rigidbody");
				drawComponent<BoxColliderComponent>(gameObject, "Box Collider");
				drawComponent<SphereColliderComponent>(gameObject, "Sphere Collider");
				drawComponent<MeshColliderComponent>(gameObject, "Mesh Collider");


				if (gameObject.hasComponent<NativeBehaviourComponent>())
					drawComponent<NativeBehaviourComponent>(gameObject, gameObject.getComponent<NativeBehaviourComponent>().behaviorName);

				if (ImGui::Button("Add Component"))
					ImGui::OpenPopup("AddComponent");

				if (ImGui::BeginPopup("AddComponent")) {
					if (ImGui::MenuItem("Camera")) {
						if (!gameObject.hasComponent<CameraComponent>())
							gameObject.addComponent<CameraComponent>();
					}
					if (ImGui::MenuItem("SpriteRenderer")) {
						if (!gameObject.hasComponent<SpriteRendererComponent>())
							gameObject.addComponent<SpriteRendererComponent>();
					}
					if (ImGui::MenuItem("Light")) {
						if (!gameObject.hasComponent<LightComponent>())
							gameObject.addComponent<LightComponent>();
					}
					if (ImGui::MenuItem("SkyBox")) {
						if (!gameObject.hasComponent<SkyBoxComponent>())
							gameObject.addComponent<SkyBoxComponent>();
					}
					if (ImGui::MenuItem("Mesh Renderer")) {
						if (!gameObject.hasComponent<MeshRendererComponent>())
							gameObject.addComponent<MeshRendererComponent>();
					}
					if (ImGui::BeginMenu("Physics")) {
						if (ImGui::MenuItem("Box Collider")) {
							if (!gameObject.hasComponent<BoxColliderComponent>())
								gameObject.addComponent<BoxColliderComponent>();
						}
						if (ImGui::MenuItem("Sphere Collider")) {
							if (!gameObject.hasComponent<SphereColliderComponent>())
								gameObject.addComponent<SphereColliderComponent>();
						}
						if (ImGui::MenuItem("Mesh Collider")) {
							if (!gameObject.hasComponent<MeshColliderComponent>())
								gameObject.addComponent<MeshColliderComponent>();
						}
						if (ImGui::MenuItem("Rigidbody")) {
							if (!gameObject.hasComponent<RigidbodyComponent>())
								gameObject.addComponent<RigidbodyComponent>();
						}
						ImGui::EndMenu();
					}
					ImGui::EndPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete")) {
					gameObject.m_scene->destroyGameObject(gameObject);
					ImGui::End();
					return false;
				}

			}

		}
		ImGui::End();
		return true;
	}
	template <typename T>
	void EditorInspectorPanel::drawComponent(GameObject gameObject, std::string name, bool removeable) {
		ST_PROFILING_FUNCTION();
		if (gameObject.hasComponent<T>()) {
			T& comp = gameObject.getComponent<T>();
			if (ImGui::TreeNodeEx((void*)typeid(T).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, name.c_str())) {
				ComponentsRender::drawComponent<T>(gameObject, comp);
				if (removeable)
					if (ImGui::Button("Remove"))
						gameObject.removeComponent<T>();
				ImGui::TreePop();
			}
			ImGui::Separator();
		}
	}
}