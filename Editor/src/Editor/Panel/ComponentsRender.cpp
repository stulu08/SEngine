#include "ComponentsRender.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

namespace Stulu {
	const ImVec2 operator *(const ImVec2& vec, const float v) {
		vec.x* v;
		vec.y* v;
		return vec;
	}

	template<typename T>
	void ComponentsRender::drawComponent(GameObject gameObject, T& component) {}
	template<>
	void ComponentsRender::drawComponent<GameObjectBaseComponent>(GameObject gameObject, GameObjectBaseComponent& component) {
		drawStringControl("Name", gameObject.getComponent<GameObjectBaseComponent>().name);
		drawStringControl("Tag", gameObject.getComponent<GameObjectBaseComponent>().tag);
		ImGui::Separator();	
	}
	template<>
	void ComponentsRender::drawComponent<TransformComponent>(GameObject gameObject, TransformComponent& component) {
		drawVector3Control("Position", component.position);
		glm::vec3 rotation = glm::degrees(component.rotation);
		drawVector3Control("Rotation", rotation);
		component.rotation = glm::radians(rotation);
		drawVector3Control("Scale", component.scale);
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
			recalc |= drawFloatControl("Zoom", component.settings.zoom);
		}
		else if (component.mode == CameraMode::Perspective) {
			recalc |= drawFloatControl("Fov", component.settings.fov);
		}
		recalc |= drawFloatControl("Far", component.settings.zFar);

		recalc |= drawFloatControl("Near", component.settings.zNear);


		drawBoolControl("Static Aspect", component.settings.staticAspect);

		if (component.settings.staticAspect) {
			recalc |= drawFloatControl("Aspect Ratio", component.settings.aspectRatio);
			if (drawIntControl("Width", *(int*)&component.settings.textureWidth)) {

				component.updateSize();
			}
			if (drawIntControl("Height", *(int*)&component.settings.textureHeight)) {

				component.updateSize();
			}
		}
		else if (recalc)
			component.updateProjection();

		drawIntControl("Depth", component.depth);

		ImGui::Image((void*)component.cam->getFrameBuffer()->getTexture()->getColorAttachmentRendereID(), ImVec2(160, 90), ImVec2(0, 1), ImVec2(1, 0));
	}
	template<>
	void ComponentsRender::drawComponent<SpriteRendererComponent>(GameObject gameObject, SpriteRendererComponent& component) {
		ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
		drawVector2Control("Tiling", component.tiling);
		drawTextureEdit("Texture", component.texture);
	}
	template<>
	void ComponentsRender::drawComponent<NativeBehaviourComponent>(GameObject gameObject, NativeBehaviourComponent& component) {
		
	}
	template<>
	void ComponentsRender::drawComponent<LightComponent>(GameObject gameObject, LightComponent& component) {
		const char* const lightTypes[] = { "Directional","Area","Spot"};
		ImGui::Combo("Type", (int*)&component.lightType, lightTypes, IM_ARRAYSIZE(lightTypes));
		ImGui::ColorEdit3("Color", glm::value_ptr(component.color));
		drawFloatControl("Strength", component.strength);
		if (component.lightType == LightComponent::Spot) {
			drawFloatControl("Cut off", component.spotLight_cutOff);
			drawFloatControl("Outer cut off", component.spotLight_outerCutOff);
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
	


	bool ComponentsRender::drawStringControl(const std::string& header, std::string& v) {
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::InputText("##String", &v);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawBoolControl(const std::string& header, bool& v) {
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::Checkbox("##CheckBoxBool", &v);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawIntControl(const std::string& header, int& v) {
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::DragInt("##INT", &v);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawFloatControl(const std::string& header, float& v) {
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::DragFloat("##FLOAT", &v, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawVector2Control(const std::string& header, glm::vec2& vec) {
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

		ImGui::Text("X");
		ImGui::SameLine();
		change |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::Text("Y");
		ImGui::SameLine();
		change |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawVector3Control(const std::string& header, glm::vec3& vec) {
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

		ImGui::Text("X");
		ImGui::SameLine();
		change |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::Text("Y");
		ImGui::SameLine();
		change |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::Text("Z");
		ImGui::SameLine();
		change |= ImGui::DragFloat("##Z", &vec.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}

	bool ComponentsRender::drawTextureEdit(const std::string& header, Ref<Texture2D>& texture) {
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		if (texture == nullptr)
			ImGui::Image((void*)Resources::getEmptySlotTexture()->getRendererID(), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
		else {
			ImGui::Image((void*)texture->getRendererID(), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_DROP_TEXTURE2D")) {
				const char* path = (const char*)payload->Data;
				texture = Texture2D::create(path);
				change = true;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
}