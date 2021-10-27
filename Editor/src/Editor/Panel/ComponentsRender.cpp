#include "ComponentsRender.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

namespace Stulu {

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
		static float zoom = .1f;
		ImGui::Image(reinterpret_cast<void*>((uint64_t)component.cam->getFrameBuffer()->getTexture()->getColorAttachmentRendereID()), ImVec2((float)component.settings.textureWidth, (float)component.settings.textureHeight)*zoom, ImVec2(0, 1), ImVec2(1, 0));
		drawFloatSliderControl("[Texture Scale]",zoom);
	}
	template<>
	void ComponentsRender::drawComponent<SkyBoxComponent>(GameObject gameObject, SkyBoxComponent& component) {
		if (component.material) {
			if (component.material->getShader())
				ImGui::Text("Material: %s(Shader: %s)", component.material->getName().c_str(), component.material->getShader()->getName().c_str());
		}
		else {
			ImGui::Text("Draw Material here");
			if (drawMaterialEdit("Material", component.material))
				component.material->uploadData();
			return;
		}

		if (drawMaterialEdit(component.material->getName(), component.material))
			component.material->uploadData();
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
		if (component.material) {
			if (component.material->getShader())
				ImGui::Text("Material: %s(Shader: %s)", component.material->getName().c_str(), component.material->getShader()->getName().c_str());
			else {
				ImGui::Text("Material: %s (Error Undefined Shader! Using default Renderer Shader)", component.material->getPath().c_str());
				if (drawMaterialEdit("Drag Material", component.material))
					component.material->uploadData();
				return;
			}
		}
		else {
			ImGui::Text("Material: Using default Renderer Shader");	
			if (drawMaterialEdit("Drag Material", component.material))
				component.material->uploadData();
			return;
		}

		if (drawMaterialEdit(component.material->getName(),component.material))
			component.material->uploadData();
	}
	template<>
	void ComponentsRender::drawComponent<MeshFilterComponent>(GameObject gameObject, MeshFilterComponent& component) {
		Ref<Mesh>& mesh = component.mesh;

		if (mesh->getSubMeshCount()) {
			ImGui::Text("SubMesh Count: %d", (int)component.mesh->getSubMeshCount());
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
			ImGui::Text("Vertices: %d", component.mesh->getVerticesCount());
			ImGui::Text("Indices: %d", component.mesh->getIndicesCount());
			ImGui::Text("Triangles: %d", component.mesh->getIndicesCount() / 3);
		}
	}
	template<>
	void ComponentsRender::drawComponent<RigidbodyComponent>(GameObject gameObject, RigidbodyComponent& component) {
		drawComboControl("Type", (int&)component.rbType, "Dynamic\0Static");
		drawBoolControl("Use gravity", component.useGravity);
		if (component.rbType == RigidbodyComponent::Dynamic) {
			//bool 3
			{
				ImGui::PushID("Enable rotation");
				bool change = false;
				ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
				ImGui::SetColumnWidth(0, 100.0f);
				ImGui::Text("Enable rotation");
				ImGui::NextColumn();

				ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

				change |= ImGui::Checkbox("##X", &component.rotationX);
				ImGui::PopItemWidth();

				ImGui::SameLine();
				change |= ImGui::Checkbox("##>", &component.rotationY);
				ImGui::PopItemWidth();

				ImGui::SameLine();
				change |= ImGui::Checkbox("##Z", &component.rotationZ);
				ImGui::PopItemWidth();

				ImGui::PopStyleVar();

				ImGui::EndColumns();

				ImGui::PopID();
			}
			drawFloatControl("Mass", component.mass);
			drawVector3Control("Center of mass", component.massCenterPos);
			drawBoolControl("Kinematic", component.kinematic);
			drawBoolControl("Retain acceleration", component.retainAccelaration);
		}
	}
	template<>
	void ComponentsRender::drawComponent<BoxColliderComponent>(GameObject gameObject, BoxColliderComponent& component) {
		drawFloatControl("Static friction", component.staticFriction);
		drawFloatControl("Dynamic friction", component.dynamicFriction);
		drawFloatControl("Restitution", component.restitution);
		drawVector3Control("Offset", component.offset);
		drawVector3Control("Size", component.size);
	}
	template<>
	void ComponentsRender::drawComponent<SphereColliderComponent>(GameObject gameObject, SphereColliderComponent& component) {
		drawFloatControl("Static friction", component.staticFriction);
		drawFloatControl("Dynamic friction", component.dynamicFriction);
		drawFloatControl("Restitution", component.restitution);
		drawVector3Control("Offset", component.offset);
		drawFloatControl("Size", component.radius);
	}
	template<>
	void ComponentsRender::drawComponent<MeshColliderComponent>(GameObject gameObject, MeshColliderComponent& component) {
		drawFloatControl("Static friction", component.staticFriction);
		drawFloatControl("Dynamic friction", component.dynamicFriction);
		drawFloatControl("Restitution", component.restitution);
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
	bool ComponentsRender::drawFloatSliderControl(const std::string& header, float& v, float min, float max) {
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
	bool ComponentsRender::drawComboControl(const std::string& header, int& current_item, const char* items_separated_by_zeros, int height_in_items) {
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
	bool ComponentsRender::drawMat4Control(const std::string& header, glm::mat4& v) {
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
			ImGui::Image(reinterpret_cast<void*>((uint64_t)EditorResources::getEmptySlotTexture()->getRendererID()), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
		else {
			ImGui::Image(reinterpret_cast<void*>((uint64_t)texture->getRendererID()), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
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
	bool ComponentsRender::drawMaterialEdit(const std::string& header, Ref<Material>& material) {
		ImGui::PushID(header.c_str());
		bool changed = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		if (material == nullptr)
			ImGui::Image(reinterpret_cast<void*>((uint64_t)EditorResources::getEmptySlotTexture()->getRendererID()), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
		else {
			ImGui::Image(reinterpret_cast<void*>((uint64_t)Previewing::get().getMaterial(material->getPath())->getRendererID()), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_DROP_MATERIAL")) {
				const char* path = (const char*)payload->Data;
				material = createRef<Material>(Material::fromDataStringPath(path));
				changed = true;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		

		if (material != nullptr && !changed) {

			if (ImGui::TreeNodeEx((void*)typeid(Material).hash_code(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick, header.c_str())) {
				MaterialData& materialData = material->getData();

				for (auto& dataType : materialData.dataTypes) {
					std::string name = dataType.name;
					Stulu::ShaderDataType type = dataType.type;
					if (type == Stulu::ShaderDataType::Float) {
						float v = std::any_cast<float>(dataType.data);
						if (drawFloatControl(name, v)) {
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
						if (drawIntControl(name, v)) {
							dataType.data = v;
							changed |= true;
						}
					}
					else if (type == Stulu::ShaderDataType::Sampler) {
						MaterialTexture& v = std::any_cast<MaterialTexture>(dataType.data);
						if (drawTextureEdit(name, v.texture)) {
							v.texPath = v.texture->getPath();
							dataType.data = v;
							changed |= true;
						}
					}
					else if (type == Stulu::ShaderDataType::SamplerCube) {
						MaterialTextureCube& v = std::any_cast<MaterialTextureCube>(dataType.data);
						for (int i = 0; i < 6; i++) {
							if (drawStringControl("Path " + std::to_string(i), v.texPath[i])) {
								dataType.data = v;
							}
						}
						if (ImGui::Button("Update")) {
							Previewing::get().addMaterial(material);
							material->toDataStringFile(material->getPath());
							material = createRef<Material>(Material::fromDataStringPath(material->getPath()));

						}
					}
					else {
						CORE_ASSERT(false, "Uknown ShaderDataType or not supported!");
					}
				}
				ImGui::TreePop();
			}
			if (changed) {
				Previewing::get().addMaterial(material);
				material->toDataStringFile(material->getPath());
			}
		}
		return changed;
	}
	
	
	Previewing::Previewing()  {
		scene = createRef<Scene>();

	}
	void Previewing::addMaterial(const Ref<Material>& material) {
		const std::string& materialPath = material->getPath();

		setUpScene(material);

		material->uploadData();
		material->getShader()->setInt("u_preview", 1);
		scene->getData().enablePhsyics3D = false;
		scene->onRuntimeStart();
		scene->onUpdateRuntime(Timestep(.05f));
		scene->onRuntimeStop();

		materials[materialPath] = camera.getComponent<CameraComponent>().cam->getFrameBuffer()->getTexture();
		scene.reset();
		material->getShader()->setInt("u_preview", 0);
	}
	Ref<Texture>& Previewing::getMaterial(const Ref<Material>& material) {
		if (!existsMaterial(material->getPath())) {
			addMaterial(material);
		}
		return materials[material->getPath()];
	}
	Ref<Texture>& Previewing::getMaterial(const std::string& path) {
		if (!existsMaterial(path)) {
			addMaterial(createRef<Material>(Material::fromDataStringPath(path)));
		}
		return materials[path];
	}
	bool Previewing::existsMaterial(const Ref<Material>& material) {
		return materials.find(material->getPath()) != materials.end();
	}
	bool Previewing::existsMaterial(const std::string& path) {
		return materials.find(path) != materials.end();
	}
	void Previewing::setUpScene(const Ref<Material>& material) {
		scene = createRef<Scene>();
		scene->onViewportResize(200, 200);

		sphere = scene->createGameObject("Sphere");
		sphere.addComponent<MeshFilterComponent>().mesh = Resources::getSphereMesh();
		sphere.getComponent<MeshRendererComponent>().material = material;
		camera = scene->createGameObject("Camera");
		light = scene->createGameObject("Light");

		camera.addComponent<CameraComponent>(CameraMode::Perspective).settings.clearColor = glm::vec4(.0f);
		camera.getComponent<GameObjectBaseComponent>().tag = "MainCam";
		camera.getComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, 1.2f);

		light.addComponent<LightComponent>(LightComponent::Directional).strength = 4;
	}
}