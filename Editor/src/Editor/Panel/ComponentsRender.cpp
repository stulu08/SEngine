#include "ComponentsRender.h"

#include "ImGui/misc/cpp/imgui_stdlib.h"

namespace Stulu {
	const ImVec2 operator *(const ImVec2& vec, const float v) {
		vec.x* v;
		vec.y* v;
		return vec;
	}

	template<typename T>
	static void ComponentsRender::drawComponent(GameObject gameObject, T& component) {}
	template<>
	static void ComponentsRender::drawComponent<GameObjectBaseComponent>(GameObject gameObject, GameObjectBaseComponent& component) {
		std::string* name = &gameObject.getComponent<GameObjectBaseComponent>().name;
		ImGui::InputText("Name", name);

		std::string* tag = &gameObject.getComponent<GameObjectBaseComponent>().tag;
		ImGui::InputText("Tag", tag);
		ImGui::Separator();
		
	}
	template<>
	void ComponentsRender::drawComponent<TransformComponent>(GameObject gameObject, TransformComponent& component) {
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetCursorPosX(80.0f);
		imGui::DragScalarFloatNoLabel("Position_3d_Transform", glm::value_ptr(component.position), 3, .1f, 0, 0, "%.3f");


		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::SetCursorPosX(80.0f);
		glm::vec3 rotation = glm::degrees(component.rotation);
		imGui::DragScalarFloatNoLabel("Rotation_3d_Transform", glm::value_ptr(rotation), 3, .1f, 0, 0, "%.3f");
		component.rotation = glm::radians(rotation);

		ImGui::Text("Scale");
		ImGui::SameLine();
		ImGui::SetCursorPosX(80.0f);
		imGui::DragScalarFloatNoLabel("Scale_3d_Transform", glm::value_ptr(component.scale), 3, .1f, 0, 0, "%.3f");
	}
	template<>
	void ComponentsRender::drawComponent<CameraComponent>(GameObject gameObject, CameraComponent& component) {
		const char* const camTypes[] = { "Perspective","Orthographic" };
		if (ImGui::Combo("Type", (int*)&component.mode, camTypes, IM_ARRAYSIZE(camTypes)))
			component.updateMode();

		const char* const clerTypes[] = { "Color","Skybox" };
		ImGui::Combo("Clear Mode", (int*)&component.settings.clearType, clerTypes, IM_ARRAYSIZE(clerTypes));
		if (component.settings.clearType == CameraComponent::Color)
			ImGui::ColorEdit4("Clear Color", glm::value_ptr(component.settings.clearColor));

		bool recalc = false;
		if (component.mode == CameraMode::Orthographic) {
			if (ImGui::DragFloat("Zoom", &component.settings.zoom))
				recalc = true;
		}
		else if (component.mode == CameraMode::Perspective) {
			if (ImGui::DragFloat("Fov", &component.settings.fov))
				recalc = true;
		}
		if (ImGui::DragFloat("Far", &component.settings.zFar))
			recalc = true;

		if (ImGui::DragFloat("Near", &component.settings.zNear))
			recalc = true;


		ImGui::Checkbox("Static Aspect Ratio", &component.settings.staticAspect);
		if (ImGui::InputFloat("Aspect Ratio", &component.settings.aspectRatio, .0f, .0f, "%.2f", !component.settings.staticAspect ? ImGuiInputTextFlags_ReadOnly : 0)) {
			/*component.settings.textureWidth /= component.settings.aspectRatio;
			component.settings.textureHeight /= component.settings.aspectRatio;
			component.cam->getFrameBuffer()->resize(component.settings.textureWidth, component.settings.textureHeight);*/
		}
		if (component.settings.staticAspect) {
			if(ImGui::InputInt("Width", (int*)&component.settings.textureWidth)){

				component.updateSize();
			}
			if (ImGui::InputInt("Height", (int*)&component.settings.textureHeight)) {

				component.updateSize();
			}
		}
		else if (recalc)
			component.updateProjection();

		ImGui::DragInt("Depth", &component.depth);

		ImGui::Image((void*)component.cam->getFrameBuffer()->getTexture()->getColorAttachmentRendereID(), ImVec2(160, 90), ImVec2(0, 1), ImVec2(1, 0));
	}
	template<>
	void ComponentsRender::drawComponent<SpriteRendererComponent>(GameObject gameObject, SpriteRendererComponent& component) {
		ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
	}
	template<>
	void ComponentsRender::drawComponent<NativeBehaviourComponent>(GameObject gameObject, NativeBehaviourComponent& component) {
		
	}
	template<>
	void ComponentsRender::drawComponent<LightComponent>(GameObject gameObject, LightComponent& component) {
		const char* const lightTypes[] = { "Directional","Area","Spot"};
		ImGui::Combo("Type", (int*)&component.lightType, lightTypes, IM_ARRAYSIZE(lightTypes));
		ImGui::ColorEdit3("Color", glm::value_ptr(component.color));
		ImGui::DragFloat("Strength", &component.stength);
		if (component.lightType == LightComponent::Spot) {
			ImGui::DragFloat("Cut off", &component.spotLight_cutOff);
			ImGui::DragFloat("Outer cut off", &component.spotLight_outerCutOff);
		}

	}
	template<>
	void ComponentsRender::drawComponent<MeshRendererComponent>(GameObject gameObject, MeshRendererComponent& component) {
		ImGui::Text("Shader: %s", component.shader->getName().c_str());
	}
	template<>
	void ComponentsRender::drawComponent<MeshFilterComponent>(GameObject gameObject, MeshFilterComponent& component) {
		Ref<Mesh>& mesh = component.mesh;

		if (mesh->getSubMeshCount()) {
			ImGui::Text("SubMesh Count: %d", (int)component.mesh->getSubMeshCount());
			for (int i = 0; i < mesh->getSubMeshCount(); i++) {
				SubMesh& m = mesh->getSubMesh(i);
				if (ImGui::TreeNodeEx((void*)(mesh->getSubMeshCount() + i + (uint32_t)gameObject), ImGuiTreeNodeFlags_OpenOnArrow, "Submesh %d", i)) {
					ImGui::Text("Vertices: %d", m.getVertices().size());
					ImGui::Text("Indices: %d", m.getIndices().size());
					ImGui::Text("Triangles: %d", m.getIndices().size() / 3);
					ImGui::TreePop();
				}
			}
		}
		else {
			ImGui::Text("Vertices: %d", component.mesh->getVertices().size());
			ImGui::Text("Indices: %d", component.mesh->getIndices().size());
			ImGui::Text("Triangles: %d", component.mesh->getIndices().size() / 3);
		}
	}
}