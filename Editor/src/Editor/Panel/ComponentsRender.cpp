#include "ComponentsRender.h"

#include "Editor/EditorApp.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

namespace Stulu {

	template<typename T>
	void ComponentsRender::drawComponent(GameObject gameObject, T& component) {}
	template<>
	void ComponentsRender::drawComponent<GameObjectBaseComponent>(GameObject gameObject, GameObjectBaseComponent& component) {
		ST_PROFILING_FUNCTION();
		ImGui::Text(std::to_string((uint32_t)gameObject).c_str());
		drawStringControl("Name", gameObject.getComponent<GameObjectBaseComponent>().name);
		drawStringControl("Tag", gameObject.getComponent<GameObjectBaseComponent>().tag);
		
		ImGui::Separator();
	}
	template<>
	void ComponentsRender::drawComponent<TransformComponent>(GameObject gameObject, TransformComponent& transform) {
		ST_PROFILING_FUNCTION();
		bool updPhysx = false;
		updPhysx |= drawVector3Control("Position", transform.position);
		updPhysx |= drawVector3Control("Rotation", transform.eulerAnglesDegrees);
		drawVector3Control("Scale", transform.scale);
		transform.rotation = Math::EulerToQuaternion(glm::radians(transform.eulerAnglesDegrees));

		if (updPhysx && getEditorLayer().isRuntime() && getEditorScene()->getData().enablePhsyics3D) {
			getEditorScene()->updateTransformAndChangePhysicsPositionAndDoTheSameWithAllChilds(gameObject);
		}
	}
	template<>
	void ComponentsRender::drawComponent<CameraComponent>(GameObject gameObject, CameraComponent& component) {
		ST_PROFILING_FUNCTION();
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
		static bool showPreview = false;
		std::string popupName = std::string(gameObject.getComponent<GameObjectBaseComponent>().name + " Preview");
		if (ImGui::Button("Show Preview"))
			ImGui::OpenPopup(popupName.c_str());
		if (ImGui::BeginPopup(popupName.c_str())) {
			ImGui::Image(reinterpret_cast<void*>((uint64_t)component.getTexture()->getColorAttachmentRendereID()), ImVec2((float)component.settings.textureWidth, (float)component.settings.textureHeight) * 0.25f, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndPopup();
		}
		
	}
	template<>
	void ComponentsRender::drawComponent<SkyBoxComponent>(GameObject gameObject, SkyBoxComponent& component) {
		ST_PROFILING_FUNCTION();
		UUID uuid = UUID::null;
		if (component.texture) {
			if (AssetsManager::existsAndType(component.texture->uuid, AssetType::SkyBox))
				uuid = component.texture->uuid;
			else
				component.texture = nullptr;
		}
		else
			ImGui::Text("Drag texture");
		if (drawTextureEdit("Texture", uuid, AssetType::SkyBox)) {
			if (uuid != UUID::null && AssetsManager::exists(uuid))
				component.texture = std::any_cast<Ref<SkyBox>&>(AssetsManager::get(uuid).data);
			else
				component.texture = nullptr;
		}
		drawComboControl("Map Type", (int&)component.mapType, "Environment Map\0Irradiance Map\0Prefilter Map");

	}
	template<>
	void ComponentsRender::drawComponent<SpriteRendererComponent>(GameObject gameObject, SpriteRendererComponent& component) {
		ST_PROFILING_FUNCTION();
		ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
		drawVector2Control("Tiling", component.tiling);
		UUID uuid = UUID::null;
		if (component.texture) {
			if(AssetsManager::existsAndType(component.texture->uuid, AssetType::Texture2D))
				uuid = component.texture->uuid;
			else
				component.texture = nullptr;
		}
		else
			ImGui::Text("Drag texture");
		if (drawTextureEdit("Texture", uuid)) {
			if (uuid != UUID::null && AssetsManager::exists(uuid))
				component.texture = std::any_cast<Ref<Texture2D>&>(AssetsManager::get(uuid).data);
			else
				component.texture = nullptr;
		}
	}
	template<>
	void ComponentsRender::drawComponent<NativeBehaviourComponent>(GameObject gameObject, NativeBehaviourComponent& component) {
		ST_PROFILING_FUNCTION();
		if(component.instance)
			component.instance->uiFunc();
	}
	template<>
	void ComponentsRender::drawComponent<LightComponent>(GameObject gameObject, LightComponent& component) {
		ST_PROFILING_FUNCTION();
		const char* const lightTypes[] = { "Directional","Area","Spot" };
		ImGui::Combo("Type", (int*)&component.lightType, lightTypes, IM_ARRAYSIZE(lightTypes));
		ImGui::ColorEdit3("Color", glm::value_ptr(component.color));
		drawFloatControl("Strength", component.strength);
		if (component.lightType == LightComponent::Spot) {
			drawFloatControl("Cut off", component.spotLight_cutOff);
			drawFloatControl("Outer cut off", component.spotLight_outerCutOff);
		}
		else if (component.lightType == LightComponent::Area) {
			drawFloatControl("Radius", component.areaRadius);
		}

	}
	template<>
	void ComponentsRender::drawComponent<MeshRendererComponent>(GameObject gameObject, MeshRendererComponent& component) {
		ST_PROFILING_FUNCTION();
		drawComboControl("CullMode", (int&)component.cullmode, {"Back","Front","Both"});

		UUID id = UUID::null;
		if (component.material && AssetsManager::exists(component.material->getUUID())) {
			ImGui::Text("Material: %s (Shader: %s)", component.material->getName().c_str(), component.material->getShader()->getName().c_str());
			id = component.material->getUUID();
		}
		else {
			ImGui::Text("Material: Using default Renderer Shader");
			if (drawMaterialEdit("Drag Material", id)) {
				component.material = AssetsManager::get(id).data._Cast<Material>();
				component.material->bind();
			}
			return;
		}
		if (drawMaterialEdit(component.material->getName(), id)) {
			component.material = AssetsManager::get(id).data._Cast<Material>();
			component.material->bind();
		}
	}
	template<>
	void ComponentsRender::drawComponent<MeshFilterComponent>(GameObject gameObject, MeshFilterComponent& component) {
		ST_PROFILING_FUNCTION();

		if (drawMeshEdit("Drag Mesh", component.mesh.uuid)) {
			if (AssetsManager::existsAndType(component.mesh.uuid, AssetType::Mesh)) {
				component.mesh = std::any_cast<MeshAsset>(AssetsManager::get(component.mesh.uuid).data);
			}
		}
		if (component.mesh.hasMesh) {
			Ref<Mesh>& mesh = component.mesh.mesh;
			if (mesh->getSubMeshCount()) {
				ImGui::Text("SubMesh Count: %d", (int)component.mesh.mesh->getSubMeshCount());
				for (int i = 0; i < mesh->getSubMeshCount(); i++) {
					SubMesh& m = mesh->getSubMesh(i);
					if (ImGui::TreeNodeEx((void*)(mesh->getSubMeshCount() + i + (uint32_t)gameObject), ImGuiTreeNodeFlags_OpenOnArrow, "Submesh %d", i)) {
						ImGui::Text("Vertices: %d", m.getVerticesCount());
						ImGui::Text("Indices: %d", m.getIndicesCount());
						ImGui::Text("Triangles: %d", m.getIndicesCount() / 3);
						ImGui::TreePop();
					}
				}
			}
			else {
				ImGui::Text("Vertices: %d", component.mesh.mesh->getVerticesCount());
				ImGui::Text("Indices: %d", component.mesh.mesh->getIndicesCount());
				ImGui::Text("Triangles: %d", component.mesh.mesh->getIndicesCount() / 3);
			}
		}
		else {
			ImGui::Text("Not a valid mesh drag one here");
		}
	}
	template<>
	void ComponentsRender::drawComponent<RigidbodyComponent>(GameObject gameObject, RigidbodyComponent& component) {
		ST_PROFILING_FUNCTION();
		bool change = false;
		change |= drawBoolControl("Use gravity", component.useGravity);
		//bool 3
		{
			ImGui::PushID("Linear Velocity");
			ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("Linear Velocity");
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

			change |= ImGui::Checkbox("##X", &component.moveX);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			change |= ImGui::Checkbox("##Y", &component.moveY);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			change |= ImGui::Checkbox("##Z", &component.moveZ);
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::EndColumns();

			ImGui::PopID();
		}
		//bool 3
		{
			ImGui::PushID("Angular Velocity");
			ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("Angular Velocity");
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

			change |= ImGui::Checkbox("##X", &component.rotationX);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			change |= ImGui::Checkbox("##Y", &component.rotationY);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			change |= ImGui::Checkbox("##Z", &component.rotationZ);
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::EndColumns();

			ImGui::PopID();
		}
		change |= drawFloatControl("Mass", component.mass);
		change |= drawVector3Control("Center of mass", component.massCenterPos);
		change |= drawBoolControl("Kinematic", component.kinematic);
		change |= drawBoolControl("Retain acceleration", component.retainAccelaration);
		if (change)
			component.updateFlags();
	}
	template<>
	void ComponentsRender::drawComponent<BoxColliderComponent>(GameObject gameObject, BoxColliderComponent& component) {
		ST_PROFILING_FUNCTION();
		drawFloatControl("Static friction", component.staticFriction);
		drawFloatControl("Dynamic friction", component.dynamicFriction);
		drawFloatControl("Restitution", component.restitution);
		drawVector3Control("Offset", component.offset);
		drawVector3Control("Size", component.size);
	}
	template<>
	void ComponentsRender::drawComponent<SphereColliderComponent>(GameObject gameObject, SphereColliderComponent& component) {
		ST_PROFILING_FUNCTION();
		drawFloatControl("Static friction", component.staticFriction);
		drawFloatControl("Dynamic friction", component.dynamicFriction);
		drawFloatControl("Restitution", component.restitution);
		drawVector3Control("Offset", component.offset);
		drawFloatControl("Size", component.radius);
	}
	template<>
	void ComponentsRender::drawComponent<CapsuleColliderComponent>(GameObject gameObject, CapsuleColliderComponent& component) {
		ST_PROFILING_FUNCTION();
		drawFloatControl("Static friction", component.staticFriction);
		drawFloatControl("Dynamic friction", component.dynamicFriction);
		drawFloatControl("Restitution", component.restitution);
		drawVector3Control("Offset", component.offset);
		drawFloatControl("Height", component.height);
		drawFloatControl("Radius", component.radius);
		drawBoolControl("Horizontal", component.horizontal);
	}
	template<>
	void ComponentsRender::drawComponent<MeshColliderComponent>(GameObject gameObject, MeshColliderComponent& component) {
		ST_PROFILING_FUNCTION();
		if (drawMeshEdit("Drag Mesh", component.mesh.uuid)) {
			if (AssetsManager::existsAndType(component.mesh.uuid, AssetType::Mesh)) {
				component.mesh = std::any_cast<MeshAsset>(AssetsManager::get(component.mesh.uuid).data);
				component.convexMesh = nullptr;
			}
		}
		if (component.mesh.hasMesh && component.convexMesh == nullptr) {
			if (component.mesh.mesh->getVerticesCount() > 255 * 3) {
				component.convexMesh = createRef<Mesh>(Mesh::copyAndLimit(component.mesh.mesh, 255 * 3));
			}
			else {
				component.convexMesh = component.mesh.mesh;
			}
		}
		if (component.mesh.hasMesh && component.convexMesh) {
			
			Ref<Mesh>& mesh = (component.convex ? component.convexMesh : component.mesh.mesh);

			if (mesh->getSubMeshCount()) {
				ImGui::Text("SubMesh Count: %d", (int)component.mesh.mesh->getSubMeshCount());
				for (int i = 0; i < mesh->getSubMeshCount(); i++) {
					SubMesh& m = mesh->getSubMesh(i);
					if (ImGui::TreeNodeEx((void*)(mesh->getSubMeshCount() + i + (uint32_t)gameObject), ImGuiTreeNodeFlags_OpenOnArrow, "Submesh %d", i)) {
						ImGui::Text("Vertices: %d", m.getVerticesCount());
						ImGui::Text("Indices: %d", m.getIndicesCount());
						ImGui::Text("Triangles: %d", m.getIndicesCount() / 3);
						ImGui::TreePop();
					}
				}
			}
			else {
				ImGui::Text("Vertices: %d", mesh->getVerticesCount());
				ImGui::Text("Indices: %d", mesh->getIndicesCount());
				ImGui::Text("Triangles: %d", mesh->getIndicesCount() / 3);
			}
		}
		else {
			component.convexMesh = nullptr;
			ImGui::Text("Not a valid mesh drag one here");
		}

		drawBoolControl("Convex", component.convex);
		drawFloatControl("Static friction", component.staticFriction);
		drawFloatControl("Dynamic friction", component.dynamicFriction);
		drawFloatControl("Restitution", component.restitution);
	}



	bool ComponentsRender::drawStringControl(const std::string& header, std::string& v) {
		ST_PROFILING_FUNCTION();
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
		ST_PROFILING_FUNCTION();
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
		ST_PROFILING_FUNCTION();
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
	bool ComponentsRender::drawInt3Control(const std::string& header, int& x, int& y, int& z) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

		change |= ImGui::DragInt("##X", &x);
		ImGui::PopItemWidth();

		ImGui::SameLine();
		change |= ImGui::DragInt("##Y", &y);
		ImGui::PopItemWidth();

		ImGui::SameLine();
		change |= ImGui::DragInt("##Z", &z);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawUIntControl(const std::string& header, uint32_t& v) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		int i = v;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::DragInt("##UINT", &i,1,0);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();
		if (change)
			v = glm::max(i,0);
		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawIntSliderControl(const std::string& header, int& v, int min, int max) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::SliderInt("##INTSLIDER", &v, min, max);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawFloatControl(const std::string& header, float& v, float min, float max) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::DragFloat("##FLOAT", &v, 0.1f, min, max, "%.2f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawFloatSliderControl(const std::string& header, float& v, float min, float max) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::SliderFloat("##FLOATSLIDER", &v, min, max, "%.2f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawVector2Control(const std::string& header, glm::vec2& vec) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });


		change |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawVector3Control(const std::string& header, glm::vec3& vec) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

		change |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		change |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		change |= ImGui::DragFloat("##Z", &vec.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawVector4Control(const std::string& header, glm::vec4& vec) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

		change |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##Z", &vec.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##W", &vec.w, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawVector4Control(const std::string& header, glm::quat& quat) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

		change |= ImGui::DragFloat("##X", &quat.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##Y", &quat.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##Z", &quat.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##W", &quat.w, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawComboControl(const std::string& header, int& current_item, const char* items_separated_by_zeros, int height_in_items) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::Combo("##COMBO", &current_item, items_separated_by_zeros, height_in_items);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawComboControl(const std::string& header, int& current_item, const std::vector<std::string>& items) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		if (ImGui::BeginCombo(header.c_str(), items[current_item].c_str())) {
			for (int i = 0; i < items.size(); i++) {
				bool selected = items[current_item] == items[i];
				if (ImGui::Selectable(items[i].c_str(), selected)) {
					auto itr = std::find(items.begin(), items.end(), items[i]);
					if (itr != items.cend())
						current_item = (uint32_t)std::distance(items.begin(), itr);
					else
						current_item = 0;
					change = true;
				}
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawMat4Control(const std::string& header, glm::mat4& v) {
		ST_PROFILING_FUNCTION();
		bool change = false;
		ImGui::Text(header.c_str());
		for (int i = 0; i < 4; i++) {
			glm::vec4 vec = v[i];
			ImGui::PushID(("Row " + std::to_string(i+1)).c_str());
			ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text(("Row " + std::to_string(i + 1)).c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });


			change |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			change |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();


			change |= ImGui::DragFloat("##Z", &vec.z, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();


			change |= ImGui::DragFloat("##W", &vec.w, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::EndColumns();

			ImGui::PopID();
		}

		return change;
	}
	void ComponentsRender::drawHelpMarker(const char* desc) {
		ST_PROFILING_FUNCTION();
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	bool ComponentsRender::drawTextureEdit(const std::string& header, UUID& uuid, AssetType type) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		void* texture = reinterpret_cast<void*>((uint64_t)EditorResources::getEmptySlotTexture()->getRendererID());
		if (AssetsManager::exists(uuid))
			switch (AssetsManager::getAssetType(uuid))
			{
			case AssetType::Texture:
				texture = reinterpret_cast<void*>((uint64_t)std::any_cast<Ref<Texture2D>>(AssetsManager::get(uuid).data)->getRendererID());
				break;
			case AssetType::Texture2D:
				texture = reinterpret_cast<void*>((uint64_t)std::any_cast<Ref<Texture2D>>(AssetsManager::get(uuid).data)->getRendererID());
				break;
			case AssetType::SkyBox:
				ImGui::Text("SkyBox");
				texture = reinterpret_cast<void*>((uint64_t)Previewing::get().get(uuid)->getRendererID());
				break;
			default:
				break;
			}


		ImGui::Image(texture, ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)type)).c_str())) {
				UUID id = *(UUID*)payload->Data;
				uuid = id;
				change = true;
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Remove")) {
			uuid = UUID::null;
			change = true;
		}

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawMaterialEdit(const std::string& header,  UUID& uuid, bool canChange) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool changed = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		bool exists = uuid != UUID::null && AssetsManager::existsAndType(uuid, AssetType::Material);
		if (!exists)
			ImGui::Image(reinterpret_cast<void*>((uint64_t)EditorResources::getEmptySlotTexture()->getRendererID()), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
		else {
			ImGui::Image(reinterpret_cast<void*>((uint64_t)Previewing::get().get(uuid)->getRendererID()), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
		}
		if (canChange) {
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Material)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					uuid = id;
					changed = true;
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();

		if (exists && !changed) {
			Asset& asset = AssetsManager::get(uuid);
			Material* mat = asset.data._Cast<Material>();
			if (mat->isReadonly)
				return false;

			std::vector<MaterialDataType>& materialData = mat->getData();
			for (auto& dataType : materialData) {
				std::string name = dataType.name;
				Stulu::ShaderDataType type = dataType.type;
				if (type == Stulu::ShaderDataType::Float) {
					float v = std::any_cast<float>(dataType.data);
					if (mat->getShader()->hasProperity(name)) {
						Ref<ShaderProperity> prop = mat->getShader()->getProperity(name);
						if (prop->getType() == ShaderProperity::Type::Range) {
							Ref<ShaderProperityRange> rangeProp = std::static_pointer_cast<ShaderProperityRange>(prop);
							if (drawFloatSliderControl(name, v, rangeProp->getMin(), rangeProp->getMax())) {
								dataType.data = v;
								changed |= true;
							}
							continue;
						}
					}
					if(drawFloatControl(name, v)) {
						dataType.data = v;
						changed |= true;
					}
					
				}
				else if (type == Stulu::ShaderDataType::Float2) {
					glm::vec2 v = std::any_cast<glm::vec2>(dataType.data);
					if (drawVector2Control(name, v)) {
						dataType.data = v;
						changed |= true;
					}
				}
				else if (type == Stulu::ShaderDataType::Float3) {
					glm::vec3 v = std::any_cast<glm::vec3>(dataType.data);
					if (drawVector3Control(name, v)) {
						dataType.data = v;
						changed |= true;
					}
				}
				else if (type == Stulu::ShaderDataType::Float4) {
					glm::vec4 v = std::any_cast<glm::vec4>(dataType.data);
					if (mat->getShader()->hasProperity(name)) {
						Ref<ShaderProperity> prop = mat->getShader()->getProperity(name);
						if (prop->getType() == ShaderProperity::Type::Color4) {
							Ref<ShaderProperityColor4> rangeProp = std::static_pointer_cast<ShaderProperityColor4>(prop);
							if (ImGui::ColorEdit4(name.c_str(), glm::value_ptr(v))) {
								dataType.data = v;
								changed |= true;
							}
							continue;
						}
					}
					if (drawVector4Control(name, v)) {
						dataType.data = v;
						changed |= true;
					}
				}
				else if (type == Stulu::ShaderDataType::Mat4) {
					glm::mat4 v = std::any_cast<glm::mat4>(dataType.data);
					if (drawMat4Control(name, v)) {
						dataType.data = v;
						changed |= true;
					}
				}
				else if (type == Stulu::ShaderDataType::Int) {
					int v = std::any_cast<int>(dataType.data);
					if (mat->getShader()->hasProperity(name)) {
						Ref<ShaderProperity> prop = mat->getShader()->getProperity(name);
						if (prop->getType() == ShaderProperity::Type::Enum) {
							Ref<ShaderProperityEnum> enumProp = std::static_pointer_cast<ShaderProperityEnum>(prop);
							if (drawComboControl(name, v, enumProp->getNames())) {
								dataType.data = v;
								changed |= true;
							}
							continue;
						}
					}
					if (drawIntControl(name, v)) {
						dataType.data = v;
						changed |= true;
					}
				}
				else if (type == Stulu::ShaderDataType::Sampler) {
					auto& t = std::any_cast<MaterialTexture&>(dataType.data);
					UUID tUUID = t.uuid;
					if (drawTextureEdit(name, tUUID, (AssetType)t.type)) {
						std::any_cast<MaterialTexture&>(dataType.data).uuid = tUUID;
						changed |= true;
					}

				}
				else {
					ST_WARN("Uknown ShaderDataType or not supported: {0}", name);
				}
				if (mat->getShader()->hasProperity(name)) {
					Ref<ShaderProperity> prop = mat->getShader()->getProperity(name);
					if (prop->getType() == ShaderProperity::Type::Marker) {
						ImGui::SameLine(); ComponentsRender::drawHelpMarker(std::static_pointer_cast<ShaderProperityMarker>(prop)->getText().c_str());
					}
				}
			}

			changed |= drawBoolControl("Is Transparent",mat->isTransparent);
			ImGui::SameLine(); ComponentsRender::drawHelpMarker("This is used by the Renderer.\n"
																"If true, the mesh will be rendered in the transparency rendering pass.\n"
																"If false, the mesh will be rendered in the opaque rendering pass.");
			if (changed) {
				mat->update(materialData);
				mat->toDataStringFile(asset.path);
				Previewing::get().add(asset);
			}
		}
		return changed;
	}

	bool ComponentsRender::drawMeshEdit(const std::string& header, UUID& uuid) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::Text(header.c_str());

		ImGui::Image(reinterpret_cast<void*>((uint64_t)EditorResources::getObjectTexture()->getRendererID()), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Mesh)).c_str())) {
				UUID id = *(UUID*)payload->Data;
				uuid = id;
				change = true;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PopID();
		return change;
	}

	template<typename T>
	bool ComponentsRender::drawControl(const std::string& header, T& value) {}
	template<>
	bool ComponentsRender::drawControl(const std::string& header, float& value) { return drawFloatControl(header, value); }
	template<>
	bool ComponentsRender::drawControl(const std::string& header, int32_t& value) { return drawIntControl(header, value); }
	template<>
	bool ComponentsRender::drawControl(const std::string& header, uint32_t& value) { return drawUIntControl(header, value); }
	template<>
	bool ComponentsRender::drawControl(const std::string& header, std::string& value) { return drawStringControl(header, value); }
	template<>
	bool ComponentsRender::drawControl(const std::string& header, glm::vec2& value) { return drawVector2Control(header, value); }
	template<>
	bool ComponentsRender::drawControl(const std::string& header, glm::vec3& value) { return drawVector3Control(header, value); }
	template<>
	bool ComponentsRender::drawControl(const std::string& header, glm::vec4& value) { return drawVector4Control(header, value); }
	template<>
	bool ComponentsRender::drawControl(const std::string& header, glm::mat4& value) { return drawMat4Control(header, value); }
	template<>
	bool ComponentsRender::drawControl(const std::string& header, bool& value) { return drawBoolControl(header, value); }


	Previewing::Previewing()  {
		ST_PROFILING_FUNCTION();
		scene = createRef<Scene>();

	}
	void Previewing::add(Asset& asset) {
		ST_PROFILING_FUNCTION();
		if (!AssetsManager::exists(asset.uuid))
			return;
		setUpScene(asset);
		scene->getData().enablePhsyics3D = false;
		scene->getData().useReflectionMapReflections = false;
		scene->onRuntimeStart();
		scene->onUpdateRuntime(Timestep(.05f));
		scene->onRuntimeStop();

		items[asset.uuid] = camera.getComponent<CameraComponent>().getTexture();
		SceneRenderer::init();
	}
	Ref<Texture>& Previewing::get(const UUID& id) {
		ST_PROFILING_FUNCTION();
		if (!exists(id)) {
			add(AssetsManager::get(id));
		}
		return items[id];
	}
	bool Previewing::exists(const UUID& id) {
		ST_PROFILING_FUNCTION();
		return items.find(id) != items.end();
	}
	void Previewing::setUpScene(Asset& asset) {
		ST_PROFILING_FUNCTION();
		scene = createRef<Scene>();
		scene->onViewportResize(200, 200);

		sphere = scene->createGameObject("Sphere");
		if(asset.type == AssetType::Material)
			sphere.addComponent<MeshRendererComponent>().material = asset.data._Cast<Material>();
		else
			sphere.addComponent<MeshRendererComponent>().material = Resources::getReflectiveMaterial();
		sphere.getComponent<MeshFilterComponent>().mesh = Resources::getSphereMeshAsset();
		camera = scene->createGameObject("Camera");
		light = scene->createGameObject("Light");


		camera.addComponent<CameraComponent>(CameraMode::Perspective).settings.clearColor = glm::vec4(.0f);
		camera.getComponent<GameObjectBaseComponent>().tag = "MainCam";
		camera.getComponent<CameraComponent>().settings.clearType = CameraComponent::ClearType::Color;
		camera.getComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, 1.2f);

		light.addComponent<LightComponent>(LightComponent::Directional).strength = 4;
		if (asset.type == AssetType::SkyBox){
			camera.addComponent<SkyBoxComponent>().texture = std::any_cast<Ref<SkyBox>>(asset.data); 
			camera.getComponent<SkyBoxComponent>().texture->bind();
			camera.getComponent<CameraComponent>().settings.clearType = CameraComponent::ClearType::Skybox;
		}
		sphere.getComponent<MeshRendererComponent>().material->bind();
	}
}