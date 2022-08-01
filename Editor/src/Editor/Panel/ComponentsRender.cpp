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


		recalc |= drawBoolControl("Is Render Target", component.settings.isRenderTarget);

		if (component.settings.isRenderTarget) {
			recalc |= drawFloatControl("Aspect Ratio", component.settings.aspectRatio);
			if (drawUIntControl("Width", component.settings.textureWidth)) {

				component.updateSize();
			}
			if (drawUIntControl("Height", component.settings.textureHeight)) {

				component.updateSize();
			}
			if(drawTextureEdit("Render Texture", component.renderTexture, false))
				component.updateSize();
		}
		else {
			if (drawIntControl("Depth", component.depth))
				component.depth = glm::max(component.depth, 0);
		}
		if (recalc)
			component.updateProjection();

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
		if (drawTextureEdit("Texture", uuid, true)) {
			if (uuid != UUID::null && AssetsManager::exists(uuid))
				component.texture = std::dynamic_pointer_cast<SkyBox>(std::any_cast<Ref<Texture>>(AssetsManager::get(uuid).data));
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
			Asset asset;
			if (AssetsManager::saveGet(component.texture->uuid, asset)) {
				if(asset.type == AssetType::Texture2D || asset.type == AssetType::RenderTexture)
					uuid = component.texture->uuid;
				else
					component.texture = nullptr;
			}
			else
				component.texture = nullptr;
		}
		else
			ImGui::Text("Drag texture");
		if (drawTextureEdit("Texture", uuid)) {
			if (uuid != UUID::null && AssetsManager::exists(uuid))
				component.texture = std::any_cast<Ref<Texture>>(AssetsManager::get(uuid).data);
			else
				component.texture = nullptr;
		}
	}
	template<>
	void ComponentsRender::drawComponent<CircleRendererComponent>(GameObject gameObject, CircleRendererComponent& component) {
		ST_PROFILING_FUNCTION();
		ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
		drawFloatControl("Thickness", component.thickness, 0.0f, 1.0f, 0.025f);
		drawFloatControl("Fade", component.fade, .0f, 1.0f, 0.00025f);
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
		if(drawFloatControl("Strength", component.strength))
			component.strength = glm::max(component.strength, .0f);
		if (component.lightType == LightComponent::Spot) {
			if (drawFloatControl("Cut off", component.spotLight_cutOff))
				component.spotLight_cutOff = glm::max(component.spotLight_cutOff, .0f);
			if (drawFloatControl("Outer cut off", component.spotLight_outerCutOff))
				component.spotLight_outerCutOff = glm::max(component.spotLight_outerCutOff, .0f);
		}
		else if (component.lightType == LightComponent::Area) {
			if (drawFloatControl("Radius", component.areaRadius))
				component.areaRadius = glm::max(component.areaRadius, .0f);
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
				component.material = std::any_cast<Ref<Material>>(AssetsManager::get(id).data);
				component.material->uploadData();
			}
			return;
		}
		if (drawMaterialEdit(component.material->getName(), id)) {
			component.material = std::any_cast<Ref<Material>>(AssetsManager::get(id).data);
			component.material->uploadData();
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
	bool ComponentsRender::drawUIntSliderControl(const std::string& header, uint32_t& v, uint32_t min, uint32_t max) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();
		int i = v;
		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::SliderInt("##UINTSLIDER", &i, min, max);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();
		if (change)
			v = glm::max(i, 0);
		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawFloatControl(const std::string& header, float& v, float min, float max, float speed) {
		ST_PROFILING_FUNCTION();
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::DragFloat("##FLOAT", &v, speed, min, max, "%.2f");
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
	bool ComponentsRender::drawHDRColorEdit(const std::string& header, glm::vec4& color) {
		using namespace ImGui;
		ImGuiWindow* window = GetCurrentWindow();
		ImGuiColorEditFlags flags = 0;
		const char label = header.c_str();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const float square_sz = GetFrameHeight();
		const float w_full = CalcItemWidth();
		const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
		const float w_inputs = w_full - w_button;
		const char* label_display_end = FindRenderedTextEnd(label);
		g.NextItemData.ClearFlags();

		BeginGroup();
		PushID(label);

		// If we're not showing any slider there's no point in doing any HSV conversions
		const ImGuiColorEditFlags flags_untouched = flags;
		if (flags & ImGuiColorEditFlags_NoInputs)
			flags = (flags & (~ImGuiColorEditFlags__DisplayMask)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

		// Context menu: display and modify options (before defaults are applied)
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			ColorEditOptionsPopup(col, flags);

		// Read stored options
		if (!(flags & ImGuiColorEditFlags__DisplayMask))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags__DisplayMask);
		if (!(flags & ImGuiColorEditFlags__DataTypeMask))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags__DataTypeMask);
		if (!(flags & ImGuiColorEditFlags__PickerMask))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags__PickerMask);
		if (!(flags & ImGuiColorEditFlags__InputMask))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags__InputMask);
		flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags__DisplayMask | ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask | ImGuiColorEditFlags__InputMask));
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__DisplayMask)); // Check that only 1 is selected
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__InputMask));   // Check that only 1 is selected

		const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
		const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
		const int components = alpha ? 4 : 3;

		// Convert to the formats we need
		float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
		if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
			ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
		else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
		{
			// Hue is lost when converting from greyscale rgb (saturation=0). Restore it.
			ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
			if (memcmp(g.ColorEditLastColor, col, sizeof(float) * 3) == 0)
			{
				if (f[1] == 0)
					f[0] = g.ColorEditLastHue;
				if (f[2] == 0)
					f[1] = g.ColorEditLastSat;
			}
		}
		int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

		bool value_changed = false;
		bool value_changed_as_float = false;

		const ImVec2 pos = window->DC.CursorPos;
		const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
		window->DC.CursorPos.x = pos.x + inputs_offset_x;

		if ((flags & (ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV)) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
		{
			// RGB/HSV 0..255 Sliders
			const float w_item_one = ImMax(1.0f, IM_FLOOR((w_inputs - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
			const float w_item_last = ImMax(1.0f, IM_FLOOR(w_inputs - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

			const bool hide_prefix = (w_item_one <= CalcTextSize((flags & ImGuiColorEditFlags_Float) ? "M:0.000" : "M:000").x);
			static const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
			static const char* fmt_table_int[3][4] =
			{
				{   "%3d",   "%3d",   "%3d",   "%3d" }, // Short display
				{ "R:%3d", "G:%3d", "B:%3d", "A:%3d" }, // Long display for RGBA
				{ "H:%3d", "S:%3d", "V:%3d", "A:%3d" }  // Long display for HSVA
			};
			static const char* fmt_table_float[3][4] =
			{
				{   "%0.3f",   "%0.3f",   "%0.3f",   "%0.3f" }, // Short display
				{ "R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f" }, // Long display for RGBA
				{ "H:%0.3f", "S:%0.3f", "V:%0.3f", "A:%0.3f" }  // Long display for HSVA
			};
			const int fmt_idx = hide_prefix ? 0 : (flags & ImGuiColorEditFlags_DisplayHSV) ? 2 : 1;

			for (int n = 0; n < components; n++)
			{
				if (n > 0)
					SameLine(0, style.ItemInnerSpacing.x);
				SetNextItemWidth((n + 1 < components) ? w_item_one : w_item_last);

				// FIXME: When ImGuiColorEditFlags_HDR flag is passed HS values snap in weird ways when SV values go below 0.
				if (flags & ImGuiColorEditFlags_Float)
				{
					value_changed |= DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
					value_changed_as_float |= value_changed;
				}
				else
				{
					value_changed |= DragInt(ids[n], &i[n], 1.0f, 0, hdr ? 0 : 255, fmt_table_int[fmt_idx][n]);
				}
				if (!(flags & ImGuiColorEditFlags_NoOptions))
					OpenPopupContextItem("context");
			}
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

	bool ComponentsRender::drawTextureEdit(const std::string& header, UUID& uuid, bool cubeMap) {
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
			if (cubeMap) {
				ImGui::Text("SkyBox");
				texture = reinterpret_cast<void*>((uint64_t)Previewing::get().get(uuid)->getRendererID());
			}
			else
				texture = reinterpret_cast<void*>((uint64_t)std::any_cast<Ref<Texture>>(AssetsManager::get(uuid).data)->getRendererID());


		ImGui::Image(texture, ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));

		if (ImGui::BeginDragDropTarget()) {
			if (cubeMap) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::SkyBox)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					uuid = id;
					change = true;
				}
			}
			else {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Texture2D)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					uuid = id;
					change = true;
				}
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::RenderTexture)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					uuid = id;
					change = true;
				}
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
			Ref<Material>& mat = std::any_cast<Ref<Material>>(asset.data);
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
					if (mat->getShader()->hasProperity(name)) {
						Ref<ShaderProperity> prop = mat->getShader()->getProperity(name);
						if (prop->getType() == ShaderProperity::Type::Color) {
							Ref<ShaderProperityColor> rangeProp = std::static_pointer_cast<ShaderProperityColor>(prop);
							if (ImGui::ColorEdit3(name.c_str(), glm::value_ptr(v))) {
								dataType.data = v;
								changed |= true;
							}
							continue;
						}
					}
					if (drawVector3Control(name, v)) {
						dataType.data = v;
						changed |= true;
					}
				}
				else if (type == Stulu::ShaderDataType::Float4) {
					glm::vec4 v = std::any_cast<glm::vec4>(dataType.data);
					if (mat->getShader()->hasProperity(name)) {
						Ref<ShaderProperity> prop = mat->getShader()->getProperity(name);
						if (prop->getType() == ShaderProperity::Type::Color) {
							Ref<ShaderProperityColor> colorProp = std::static_pointer_cast<ShaderProperityColor>(prop);
							if (colorProp->isHDR()) {
								if (drawHDRColorEdit(name, v)) {
									dataType.data = v;
									changed |= true;
								}
							}
							else {
								if (ImGui::ColorEdit4(name.c_str(), glm::value_ptr(v))) {
									dataType.data = v;
									changed |= true;
								}
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
					if (drawTextureEdit(name, tUUID, t.type == (int)AssetType::SkyBox)) {
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
			changed |= drawComboControl("Transparency Mode", (int&)mat->getTransparencyMode(), { "Opaque","Cutout","Transparent" });
			if(mat->getTransparencyMode() == TransparencyMode::Cutout)
				changed |= drawFloatControl("Alpha Cutout", mat->getAlphaCutOff());

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


		items[asset.uuid] = camera.getComponent<CameraComponent>().getNativeCamera()->getFrameBuffer()->getTexture();
		scene.reset();
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

		camera = scene->createGameObject("Camera");
		light = scene->createGameObject("Light");
		light.addComponent<LightComponent>(LightComponent::Directional).strength = 4;
		light.getComponent<TransformComponent>().rotation = glm::quat(glm::radians(glm::vec3(-45.0f, -45.0f, .0f)));

		camera.addComponent<CameraComponent>(CameraMode::Perspective, false).settings.clearColor = glm::vec4(.0f);
		camera.getComponent<GameObjectBaseComponent>().tag = "MainCam";
		camera.getComponent<CameraComponent>().settings.clearType = CameraComponent::ClearType::Color;
		camera.getComponent<CameraComponent>().depth = 1;
		camera.getComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, 1.2f);

		if (asset.type == AssetType::SkyBox) {
			camera.addComponent<SkyBoxComponent>().texture = std::dynamic_pointer_cast<SkyBox>(std::any_cast<Ref<Texture>>(asset.data));
		}
		else {
			camera.addComponent<SkyBoxComponent>().texture = Resources::getDefaultSkyBox();
		}
		camera.getComponent<SkyBoxComponent>().texture->bind();
		camera.getComponent<CameraComponent>().settings.clearType = CameraComponent::ClearType::Skybox;

		renderObject = scene->createGameObject("Model");
		renderObject.saveAddComponent<MeshFilterComponent>().mesh = Resources::getCubeMeshAsset();
		if (asset.type == AssetType::SkyBox) {
			renderObject.saveAddComponent<MeshFilterComponent>().mesh = Resources::getSphereMeshAsset();
		}
		
		if (asset.type == AssetType::Material) {
			renderObject.saveAddComponent<MeshRendererComponent>().material = std::any_cast<Ref<Material>>(asset.data);
		}
		else {
			renderObject.saveAddComponent<MeshRendererComponent>().material = Resources::getReflectiveMaterial();
		}

		if (asset.type == AssetType::Model) {
			Model& model = std::any_cast<Model&>(asset.data);
			scene->destroyGameObject(renderObject);
			renderObject = scene->addModel(model);
			float furthestX = .0f;
			float furthestY = .0f;
			float furthestZ = .0f;
			for (MeshAsset& mesh : model.getMeshes()) {
				if (mesh.mesh) {
					glm::vec3 temp = mesh.mesh->getFurthesteachAxisFromPos(glm::vec3(.0f), 0);
					if (furthestX < temp.x)
						furthestX = temp.x;
					if (furthestY < temp.y)
						furthestY = temp.y;
					if (furthestZ < temp.z)
						furthestZ = temp.z;
				}
				
			}
			auto& transform = camera.getComponent<TransformComponent>();
			transform.position = glm::vec3(furthestX, furthestY, furthestZ);

			glm::mat4 trans = glm::lookAt(transform.position, glm::vec3(.0f), transform.up);
			Math::decomposeTransform(glm::inverse(trans), transform.position, transform.rotation, transform.scale);
		}
	}
}