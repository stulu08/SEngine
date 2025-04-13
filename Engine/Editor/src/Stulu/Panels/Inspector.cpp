#include "Inspector.h"

#include "Stulu/Panels/Hierarchy.h"
#include "Stulu/App.h"
#include "Stulu/Style.h"

#include <Stulu/Scripting/Managed/Bindings/Bindings.h>
#include <Stulu/Scripting/Managed/Bindings/Components/GameObject.h>

#include "Stulu/MainLayer.h"
using namespace Stulu;

namespace Editor {
	InspectorPanel::InspectorPanel()
		: Panel("Inspector") {
		LoadObjects();
	}

	void InspectorPanel::DrawImGui() {
		ST_PROFILING_SCOPE("ImGui - Inspector");

		auto& layer = App::get().GetLayer();
		const auto& selectedObjects = layer.GetPanel<Editor::HierarchyPanel>().GetSelected();

		if (selectedObjects.size() == 1) {
			GameObject selected = GameObject(selectedObjects[0], layer.GetActiveScene().get());

			if (!selected.IsValid())
				return;
			
			auto& base = selected.getComponent<GameObjectBaseComponent>();

			ImGui::SameLine(m_windowIndent);

			if (m_nameEditing != selected.GetID()) {
				m_nameEditing = entt::null;
				if (ImGui::Button(ICON_FK_PENCIL)) {
					m_nameEditing = selected.GetID();
				}
				ImGui::SameLine();
				ImGui::Text(base.name.c_str());
			}
			else {
				if (ImGui::Button(ICON_FK_CHECK)) {
					m_nameEditing = entt::null;
				}
				ImGui::SameLine();
				ImGui::InputText("##name", &base.name);
			}



			uint64_t id = (uint64_t)selected.GetID();
			void* args[1] = { &id };
			for (const InspectorRenderer& inspector : m_inspectors) {
				if (!StuluBindings::GameObject::hasComponent(id, inspector.GetType())) {
					continue;
				}
				const float offsetX = ImGui::GetStyle().FramePadding.x * 3.0f;

				const std::string& header = inspector.GetHeader();
				bool isHeaderID = header.rfind("##", 0) == 0;
				if (!isHeaderID) {
					if (ImGui::TreeNodeEx(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)) {
						ImGui::Indent(offsetX);
						inspector.GetScriptObject()->CallMethod(m_renderMethod, args);
						ImGui::Unindent(offsetX);
						ImGui::TreePop();
					}
				}
				else {
					ImGui::PushID(header.c_str());
					ImGui::Indent(offsetX);
					inspector.GetScriptObject()->CallMethod(m_renderMethod, args);
					ImGui::Unindent(offsetX);
					ImGui::PopID();
				}

			}
		}
		else if (selectedObjects.size() > 1) {

		}
	}

	void InspectorPanel::DrawImGuizmo() {
		auto& layer = App::get().GetLayer();
		const auto& selectedObjects = layer.GetPanel<Editor::HierarchyPanel>().GetSelected();

		for (entt::entity selected : selectedObjects) {
			uint64_t id = (uint64_t)selected;
			void* args[1] = { &id };

			for (const InspectorRenderer& inspector : m_inspectors) {
				if (!StuluBindings::GameObject::hasComponent(id, inspector.GetType())) {
					continue;
				}
				inspector.GetScriptObject()->CallMethod(m_renderGizmosMethod, args);
			}
		}
	}

	void InspectorPanel::PreWindow() {
		m_windowIndent = ImGui::GetStyle().WindowPadding.x;
		ImGui::PushStyleVarX(ImGuiStyleVar_WindowPadding, 0.0f);
	}

	void InspectorPanel::PostWindow() {
		ImGui::PopStyleVar();
	}

	bool InspectorPanel::HasInspector(Stulu::Mono::ReflectionType refType) const {
		std::string searchedName = refType.GetType().GetNameFull(Mono::TypeNameFormat::REFLECTION);

		for (const auto& inspector : m_inspectors) {
			std::string inspectorName = inspector.GetType().GetType().GetNameFull(Mono::TypeNameFormat::REFLECTION);
			if (inspectorName == searchedName)
				return true;
		}
		return false;
	}

	void InspectorPanel::LoadObjects() {
		m_inspectors.clear();

		auto& manager = App::get().getAssemblyManager();
		m_inspectorClass = manager->getScriptCoreAssembly()->CreateClass("Editor", "Inspector");
		m_inspectorAttribute = manager->getScriptCoreAssembly()->CreateClass("Editor", "InspectorRendererAttribute");
		m_renderMethod = m_inspectorClass.GetMethodFromName("Impl_Render", 1);
		m_renderGizmosMethod = m_inspectorClass.GetMethodFromName("Impl_RenderGizmos", 1);

		LoadScriptObjects(manager->getScriptCoreAssembly());
		LoadScriptObjects(manager->getAppAssembly());
	}
	void InspectorPanel::LoadScriptObjects(Ref<ScriptAssembly> assembly) {
		auto& classes = assembly->LoadAllClasses(m_inspectorClass);

		Mono::ClassField typeField = m_inspectorAttribute.GetFieldFromName("type");
		Mono::ClassField nameField = m_inspectorAttribute.GetFieldFromName("name");
		Mono::ClassField priotityField = m_inspectorAttribute.GetFieldFromName("priority");

		CORE_ASSERT(typeField, "Field 'type' not found!");
		CORE_ASSERT(nameField, "Field 'name' not found!");
		CORE_ASSERT(priotityField, "Field 'priority' not found!");

		for (Mono::Class& inspector : classes) {
			Mono::CustomAttrInfo atrrInfo = Mono::CustomAttrInfo::FromClass(inspector);
			if (atrrInfo.HasAttribute(m_inspectorAttribute)) {
				Mono::Object atrributeObject = atrrInfo.GetAttribute(m_inspectorAttribute);
				if (!atrributeObject)
					continue;

				Mono::ReflectionType reftype = nullptr;
				Mono::String header = nullptr;
				int32_t prio = 0;

				typeField.GetValue(atrributeObject, &reftype);
				nameField.GetValue(atrributeObject, &header);
				priotityField.GetValue(atrributeObject, &prio);
				
				m_inspectors.push_back(std::move(InspectorRenderer(reftype, header.ToUtf8(), inspector, prio, assembly.get())));
			}

			atrrInfo.Free();
		}

		std::sort(m_inspectors.begin(), m_inspectors.end(), [](const InspectorRenderer& left, const InspectorRenderer& right) {
			return left.GetPriority() < right.GetPriority();
		});
	}
}
