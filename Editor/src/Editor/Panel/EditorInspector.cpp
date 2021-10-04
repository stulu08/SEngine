#include "EditorInspector.h"
namespace Stulu {
	/// <summary>
	/// Returns false if gameobject was deleted
	/// </summary>
	/// <param name="gameObject"></param>
	/// <returns></returns>
	bool EditorInspectorPanel::render(GameObject gameObject) {
		ImGui::Begin("Inspector");
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
			drawComponent<SpriteRendererComponent>(gameObject, "SpriteRenderer");
			drawComponent<LightComponent>(gameObject, "Light");
			drawComponent<MeshRendererComponent>(gameObject, "MeshRenderer");
			drawComponent<MeshFilterComponent>(gameObject, "MeshFilter");


			if(gameObject.hasComponent<NativeBehaviourComponent>())
				drawComponent<NativeBehaviourComponent>(gameObject, gameObject.getComponent<NativeBehaviourComponent>().behaviorName);

			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent")) {
				if (ImGui::MenuItem("Camera")) {
					if(!gameObject.hasComponent<CameraComponent>())
						gameObject.addComponent<CameraComponent>();
				}
				if (ImGui::MenuItem("SpriteRenderer")) {
					if(!gameObject.hasComponent<SpriteRendererComponent>())
						gameObject.addComponent<SpriteRendererComponent>();
				}
				if (ImGui::MenuItem("Light")) {
					if(!gameObject.hasComponent<LightComponent>())
						gameObject.addComponent<LightComponent>();
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
		

		ImGui::End();
		return true;
	}
	template <typename T>
	void EditorInspectorPanel::drawComponent(GameObject gameObject, std::string name, bool removeable) {
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