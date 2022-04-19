#include "EditorInspector.h"
#include "Editor/EditorApp.h"
namespace Stulu {

#define SCRIPTING_UI_FUNCTION(TYPE) \
	{TYPE & value = *((TYPE*)field.value); \
	if (ComponentsRender::drawControl<TYPE>(name, value)) { \
		script->getFields()[name].value = (void*)&value; \
		script->setClassField(name); \
	}}
	MonoClass* showInEdiorAttrb;
	/// <summary>
	/// Returns false if gameobject was deleted
	/// </summary>
	/// <param name="gameObject"></param>
	/// <returns></returns>
	bool EditorInspectorPanel::render(GameObject gameObject, bool* open) {
		ST_PROFILING_FUNCTION();
		if(!showInEdiorAttrb)
			showInEdiorAttrb = Application::get().getAssemblyManager()->getScriptCoreAssembly()->createClass("Stulu", "ShowInEditorAttribute");

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


				if (gameObject.hasComponent<ScriptingComponent>()) {
					ScriptingComponent& comp = gameObject.getComponent<ScriptingComponent>();
					for (auto script : comp.runtimeScripts) {
						if (ImGui::TreeNodeEx(script->getClassName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
							for (auto&[name,field] : script->getFields()) {
								if (!MonoObjectInstance::fieldHasAttribute(field, showInEdiorAttrb))
									continue;
								script->reloadClassFieldValue(name);
								switch (field.type) {
								case MonoObjectInstance::MonoClassMember::Type::Vector4_t:
									SCRIPTING_UI_FUNCTION(glm::vec4);
									break;
								case MonoObjectInstance::MonoClassMember::Type::Vector3_t:
									SCRIPTING_UI_FUNCTION(glm::vec3);
									break;
								case MonoObjectInstance::MonoClassMember::Type::Vector2_t:
									SCRIPTING_UI_FUNCTION(glm::vec2);
									break;
								case MonoObjectInstance::MonoClassMember::Type::float_t:
									SCRIPTING_UI_FUNCTION(float);
									break;
								case MonoObjectInstance::MonoClassMember::Type::int_t:
									SCRIPTING_UI_FUNCTION(int32_t);
									break;
								case MonoObjectInstance::MonoClassMember::Type::uint_t:
									SCRIPTING_UI_FUNCTION(uint32_t);
									break;
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
				
				if (ImGui::Button("Add Component"))
					ImGui::OpenPopup("AddComponent");

				if (ImGui::BeginPopup("AddComponent")) {
					if (ImGui::MenuItem("Camera")) {
						gameObject.saveAddComponent<CameraComponent>();
					}
					if (ImGui::MenuItem("SpriteRenderer")) {
						gameObject.saveAddComponent<SpriteRendererComponent>();
					}
					if (ImGui::MenuItem("Light")) {
						gameObject.saveAddComponent<LightComponent>();
					}
					if (ImGui::MenuItem("SkyBox")) {
						gameObject.saveAddComponent<SkyBoxComponent>();
					}
					if (ImGui::MenuItem("Mesh Renderer")) {
						gameObject.saveAddComponent<MeshRendererComponent>();
					}
					if (ImGui::MenuItem("Mesh Filter")) {
						gameObject.saveAddComponent<MeshFilterComponent>();
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
					if (ImGui::BeginMenu("Scripts")) {
						for (auto& cl : getEditorApp().getProject().assembly->getClasses()) {
							std::string name = (cl.nameSpace.empty() ? "" : cl.nameSpace + ".") + cl.name;
							if (ImGui::MenuItem(name.c_str())) {
								Ref<MonoObjectInstance> object = createRef<MonoObjectInstance>(cl.nameSpace, cl.name, getEditorApp().getProject().assembly.get());
								object->loadAll();
								getEditorApp().getProject().assembly->registerObject(object);

								auto& comp = gameObject.saveAddComponent<ScriptingComponent>();
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