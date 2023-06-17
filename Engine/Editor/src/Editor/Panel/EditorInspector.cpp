#include "EditorInspector.h"
#include "Editor/EditorApp.h"
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
					ImGui::End();
					return false;
				}

			}

		}
		ImGui::End();
		return true;
	}
#define SCRIPTING_UI_FUNCTION(TYPE) \
	{TYPE & value = *((TYPE*)field.value); \
	if (ComponentsRender::drawControl<TYPE>(name, value)) { \
		script->getFields()[name].value = (void*)&value; \
		script->setClassField(name); \
	}}
	MonoClass* showInEditorAttrb;
	void EditorInspectorPanel::drawScriptingComponent(GameObject gameObject) {
		if (!showInEditorAttrb)
			showInEditorAttrb = Application::get().getAssemblyManager()->getScriptCoreAssembly()->createClass("Stulu", "ShowInEditorAttribute");
		ScriptingComponent& comp = gameObject.getComponent<ScriptingComponent>();
		for (auto script : comp.runtimeScripts) {
			if (ImGui::TreeNodeEx(script->getClassName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				for (auto& name : script->getFieldOrder()) {
					auto& field = script->getFields()[name];
					if (!MonoObjectInstance::fieldHasAttribute(field, showInEditorAttrb))
						continue;
					script->reloadClassFieldValue(name);
					if (MonoClassMemberTypeFnInfo::infos.find(field.typeName) != MonoClassMemberTypeFnInfo::infos.end()) {
						MonoClassMemberTypeFnInfo::infos.at(field.typeName).uiFunc(script, field, name);
					}
					else {
						//other
						script->getFields()[name].value = nullptr;
					}
					/*
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
					case MonoObjectInstance::MonoClassMember::Type::bool_t:
						SCRIPTING_UI_FUNCTION(bool);
						break;
					case MonoObjectInstance::MonoClassMember::Type::Other:
						script->getFields()[name].value = nullptr;
						break;
					}
					*/
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