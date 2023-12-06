#include "EditorInspector.h"
#include "Editor/EditorApp.h"
#include <imgui/imgui_internal.h>
namespace Stulu {
	/// <summary>
	/// Returns false if gameobject was deleted
	/// </summary>
	/// <param name="gameObject"></param>
	/// <returns></returns>
	bool EditorInspectorPanel::render(bool* open) {
		ST_PROFILING_FUNCTION();
		if (ImGui::Begin("Inspector", open)) {
			if (AssetMode) {
				renderAssetInspector();
			}
			else {
				GameObject gameObject = EditorHierarchyPanel::getCurrentObject();
				if (gameObject == GameObject::null) {
					ImGui::End();
					return true;
				}
				else if (!gameObject.hasComponent<GameObjectBaseComponent>()) {
					ImGui::End();
					return true;
				}
				renderGameObjectInspector();
			}
			
		}

		ImGui::End();
		return true;
	}
	void EditorInspectorPanel::renderGameObjectInspector() {
		ST_PROFILING_FUNCTION();
		GameObject& gameObject = EditorHierarchyPanel::getCurrentObject();
		auto base = gameObject.getComponent<GameObjectBaseComponent>();

		if (ImGui::CollapsingHeader(base.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

			ComponentsRender::drawComponent<GameObjectBaseComponent>(gameObject, base);
			drawComponent<TransformComponent>(gameObject, "Transform", false);
			drawComponent<CameraComponent>(gameObject, "Camera");
			drawComponent<SkyBoxComponent>(gameObject, "SkyBox");
			drawComponent<SpriteRendererComponent>(gameObject, "SpriteRenderer");
			drawComponent<CircleRendererComponent>(gameObject, "CircleRenderer");
			drawComponent<LightComponent>(gameObject, "Light");
			drawComponent<MeshRendererComponent>(gameObject, "MeshRenderer");
			drawComponent<MeshFilterComponent>(gameObject, "MeshFilter");
			drawComponent<RigidbodyComponent>(gameObject, "Rigidbody");
			drawComponent<BoxColliderComponent>(gameObject, "Box Collider");
			drawComponent<SphereColliderComponent>(gameObject, "Sphere Collider");
			drawComponent<CapsuleColliderComponent>(gameObject, "Capsule Collider");
			drawComponent<MeshColliderComponent>(gameObject, "Mesh Collider");
			drawComponent<PostProcessingComponent>(gameObject, "Post Processing");
			drawComponent<ParticleSystemComponent>(gameObject, "Particle System");

			if (gameObject.hasComponent<NativeBehaviourComponent>())
				drawComponent<NativeBehaviourComponent>(gameObject, gameObject.getComponent<NativeBehaviourComponent>().behaviorName);


			if (gameObject.hasComponent<ScriptingComponent>()) {
				drawScriptingComponent(gameObject);
			}

			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent")) {
				if (ImGui::BeginMenu("Camera Rendering")) {
					if (ImGui::MenuItem("Perspective Camera")) {
						gameObject.saveAddComponent<CameraComponent>(CameraMode::Perspective);
						gameObject.saveAddComponent<PostProcessingComponent>();
					}
					if (ImGui::MenuItem("Orthographic Camera")) {
						gameObject.saveAddComponent<CameraComponent>(CameraMode::Orthographic);
					}
					if (ImGui::MenuItem("SkyBox")) {
						gameObject.saveAddComponent<SkyBoxComponent>();
					}
					if (ImGui::MenuItem("PostProcessing")) {
						gameObject.saveAddComponent<PostProcessingComponent>();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("3D Rendering")) {
					if (ImGui::MenuItem("Light")) {
						gameObject.saveAddComponent<LightComponent>();
					}
					if (ImGui::MenuItem("Mesh Renderer")) {
						gameObject.saveAddComponent<MeshRendererComponent>();
					}
					if (ImGui::MenuItem("Mesh Filter")) {
						gameObject.saveAddComponent<MeshFilterComponent>();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("2D Rendering")) {
					if (ImGui::MenuItem("SpriteRenderer")) {
						gameObject.saveAddComponent<SpriteRendererComponent>();
					}
					if (ImGui::MenuItem("CircleRenderer")) {
						gameObject.saveAddComponent<CircleRendererComponent>();
					}
					if (ImGui::MenuItem("Particle System")) {
						gameObject.saveAddComponent<ParticleSystemComponent>();
					}
					ImGui::EndMenu();
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
					if (ImGui::MenuItem("Capsule Collider")) {
						if (!gameObject.hasComponent<CapsuleColliderComponent>())
							gameObject.addComponent<CapsuleColliderComponent>();
					}
					if (ImGui::MenuItem("Mesh Collider")) {
						if (!gameObject.hasComponent<MeshColliderComponent>())
							gameObject.addComponent<MeshColliderComponent>();
						MeshFilterComponent filter;
						if (gameObject.saveGetComponent(filter)) {
							gameObject.getComponent<MeshColliderComponent>().mesh = filter.mesh;
						}
					}
					if (ImGui::MenuItem("Rigidbody")) {
						if (!gameObject.hasComponent<RigidbodyComponent>())
							gameObject.addComponent<RigidbodyComponent>();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Scripts")) {
					for (auto& cl : getEditorApp().getProject().assembly->getClasses()) {
						std::string name = (cl.nameSpace.empty() ? "" : cl.nameSpace + ".") + cl.name;
						if (ImGui::MenuItem(name.c_str())) {
							auto& comp = gameObject.saveAddComponent<ScriptingComponent>();

							Ref<MonoObjectInstance> object = createRef<MonoObjectInstance>(cl.nameSpace, cl.name, getEditorApp().getProject().assembly.get());
							object->loadAll();
							comp.runtimeScripts.push_back(object);
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				gameObject.m_scene->destroyGameObject(gameObject);
				EditorHierarchyPanel::setSelectedGameObject(GameObject::null);
			}
		}
	}

	void EditorInspectorPanel::renderAssetInspector() {
		ST_PROFILING_FUNCTION();
		Asset& selected = AssetBrowserPanel::GetSelected();
		ImGui::TextWrapped("Asset %s", selected.path.c_str());
		ImGui::Text("UUID %llu", selected.uuid);
		ImGui::Text("Properitys:", selected.path.c_str());
		if (ImGui::BeginChild("Properitys") && selected.uuid != UUID::null) {
			if (selected.type == AssetType::Model) {
				if (ImGui::TreeNodeEx("Meshes", ImGuiTreeNodeFlags_SpanFullWidth)) {
					std::vector<MeshAsset>& meshVec = std::any_cast<Model&>(selected.data).getMeshes();
					for (int i = 0; i < meshVec.size(); i++) {
						if (meshVec[i].name.c_str()) {
							std::string name = "Mesh: " + meshVec[i].name;
							ImGui::Text(name.c_str());
							ImGui::PushID(name.c_str());
						}
						else {
							std::string name = "Mesh: " + std::to_string(i);

							ImGui::TextWrapped(name.c_str());
							ImGui::PushID(name.c_str());
						}
						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
							ImGui::SetDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Mesh)).c_str(), &meshVec[i].uuid, sizeof(UUID));
							ImGui::EndDragDropSource();
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNodeEx("Materials", ImGuiTreeNodeFlags_SpanFullWidth)) {
					auto& matVec = std::any_cast<Model&>(selected.data).getMaterials();
					for (auto& [index, mat] : matVec) {
						std::string name;
						if (mat->getName().c_str()) {
							name = "Material: " + mat->getName();
						}
						else {
							name = "Material: " + std::to_string(index);
						}
						if (ImGui::TreeNodeEx(name.c_str())) {
							ComponentsRender::drawMaterialEdit("Material", mat->getUUID(), false);
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}
			else if (selected.type == AssetType::Material) {
				ComponentsRender::drawMaterialEdit("Material", selected.uuid, false);
			}
			else if (selected.type == AssetType::Texture2D) {
				int type = (int)selected.type - 1;
				Ref<Texture2D> texture = std::dynamic_pointer_cast<Texture2D>(std::any_cast<Ref<Texture>>(selected.data));
				ComponentsRender::drawVector2Control("Tilling", texture->getSettings().tiling);
				ComponentsRender::drawEnumComboControl("Format", texture->getSettings().format);
				ComponentsRender::drawEnumComboControl("Wrap Mode", texture->getSettings().wrap);
				ComponentsRender::drawEnumComboControl("Filtering", texture->getSettings().filtering);

				if (ImGui::Button("Update")) {
					texture->update();
					AssetsManager::setProperity<TextureSettings>(selected.path, std::pair<std::string, TextureSettings> {"format", texture->getSettings()});
				}
			}
			else if (selected.type == AssetType::SkyBox) {
				int type = (int)selected.type - 1;
				static std::vector<uint32_t> ress = { 64,128,256,512,1024,2048,4096 };
				uint32_t resolution = AssetsManager::getProperity<uint32_t>(selected.path, "resolution");

				auto pos = std::find(ress.begin(), ress.end(), resolution);
				int item;
				if (pos != ress.end())
					item = (int)std::distance(ress.begin(), pos);
				else
					item = 3;

				if (ComponentsRender::drawComboControl("Resolution", item, " 64\0 128\0 256\0 512\0 1024\0 2048\0 4096")) {
					resolution = ress[item];
					AssetsManager::setProperity<uint32_t>(selected.path, { "resolution" ,ress[item] });
				}
				Ref<SkyBox>& texture = std::dynamic_pointer_cast<SkyBox>(std::any_cast<Ref<Texture>&>(selected.data));
				bool disabled = texture->getWidth() == resolution;
				if (disabled)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				if (ImGui::Button("Update")) {
					texture->update(selected.path);
				}
				if (disabled)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
			}
		}
		ImGui::EndChild();
	}

	MonoClass* showInEditorAttrb;
	void EditorInspectorPanel::drawScriptingComponent(GameObject gameObject) {
		if (!showInEditorAttrb)
			showInEditorAttrb = Application::get().getAssemblyManager()->getScriptCoreAssembly()->createClass("Stulu", "ShowInEditorAttribute");

		ScriptingComponent& comp = gameObject.getComponent<ScriptingComponent>();
		for (auto script : comp.runtimeScripts) {
			if (ImGui::TreeNodeEx(script->getClassName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				for (auto& name : script->getFieldOrder()) {
					Ref<Property> prop;
					for (auto f : script->getFields()) {
						if (f->getName() == name) {
							prop = f;
							break;
						}
					}

					if (!prop || !MonoObjectInstance::FieldHasAttribute(prop, showInEditorAttrb))
						continue;


					switch (prop->getType()) {
					case PropertyType::Int_t:{
						Ref<Int32Property> tprop = std::dynamic_pointer_cast<Int32Property>(prop);
						int32_t value = tprop->GetValue();
						if (ComponentsRender::drawControl(name, value))
							tprop->SetValue(value);
						break;
					}
					case PropertyType::UInt_t: {
						Ref<UInt32Property> tprop = std::dynamic_pointer_cast<UInt32Property>(prop);
						uint32_t value = tprop->GetValue();
						if (ComponentsRender::drawControl(name, value))
							tprop->SetValue(value);
						break;
					}
					case PropertyType::Float_t: {
						Ref<FloatProperty> tprop = std::dynamic_pointer_cast<FloatProperty>(prop);
						float value = tprop->GetValue();
						if (ComponentsRender::drawControl(name, value))
							tprop->SetValue(value);
						break;
					}
					case PropertyType::Bool_t: {
						Ref<BoolProperty> tprop = std::dynamic_pointer_cast<BoolProperty>(prop);
						bool value = tprop->GetValue();
						if (ComponentsRender::drawControl(name, value))
							tprop->SetValue(value);
						break;
					}
					case PropertyType::Vector2_t: {
						Ref<Vector2Property> tprop = std::dynamic_pointer_cast<Vector2Property>(prop);
						auto value = tprop->GetValue();
						if (ComponentsRender::drawControl(name, value))
							tprop->SetValue(value);
						break;
					}
					case PropertyType::Vector3_t: {
						Ref<Vector3Property> tprop = std::dynamic_pointer_cast<Vector3Property>(prop);
						auto value = tprop->GetValue();
						if (ComponentsRender::drawControl(name, value))
							tprop->SetValue(value);
						break;
					}
					case PropertyType::Vector4_t: {
						Ref<Vector4Property> tprop = std::dynamic_pointer_cast<Vector4Property>(prop);
						auto value = tprop->GetValue();
						if (ComponentsRender::drawControl(name, value))
							tprop->SetValue(value);
						break;
					}
					case PropertyType::AssetHandle: {
						Ref<AssetProperty> tprop = std::dynamic_pointer_cast<AssetProperty>(prop);
						auto value = tprop->GetValue();
						if (ComponentsRender::drawAssetControl(name, value, tprop->getAssetType()))
							tprop->SetValue(value);
						break;
					}
					case PropertyType::GameObject_t: {
						Ref<GameObjectProperty> tprop = std::dynamic_pointer_cast<GameObjectProperty>(prop);
						auto value = tprop->GetValue();
						if (ComponentsRender::drawGameObjectEdit(name, value))
							tprop->SetValue(value);
						break;
					}
					}
					
				}
				if (ImGui::Button("Remove")) {
					comp.runtimeScripts.erase(std::find(comp.runtimeScripts.begin(), comp.runtimeScripts.end(), script));
					ImGui::TreePop();
					ImGui::Separator();
					break;
				}
				ImGui::TreePop();
			}
			ImGui::Separator();
		}
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