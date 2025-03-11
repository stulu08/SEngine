#include "Inspector.h"

#include "Stulu/Panels/Hierarchy.h"
#include "Stulu/App.h"

#include <Stulu/Scripting/Managed/Bindings/Bindings.h>
#include <Stulu/Scripting/Managed/Bindings/Components/GameObject.h>
using namespace Stulu;

namespace Editor {
	InspectorPanel::InspectorPanel()
		: Panel("Inspector") {
		LoadObjects();
	}

	void InspectorPanel::DrawImGui() {
		auto& layer = App::get().GetLayer();
		const auto& selectedObjects = layer.GetPanel<Editor::HierarchyPanel>().GetSelected();

		if (selectedObjects.size() == 1) {
			GameObject selected = GameObject(selectedObjects[0], layer.GetActiveScene().get());

			if (!selected.IsValid())
				return;
			
			auto& base = selected.getComponent<GameObjectBaseComponent>();

			ImGui::Text(base.name.c_str());

			uint64_t id = (uint64_t)selected.GetID();
			void* args[1] = { &id };
			for (const InspectorRenderer& inspector : m_inspectors) {
				if (!StuluBindings::GameObject::hasComponent(id, inspector.GetType())) {
					continue;
				}
				if (ImGui::TreeNodeEx(inspector.GetHeader().c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)) {
					inspector.GetScriptObject()->CallMethod(m_renderMethod, args);
					ImGui::TreePop();
				}

			}
		}
		else if (selectedObjects.size() > 1) {

		}
	}

	void InspectorPanel::PreWindow() {
		ImGui::PushStyleVarX(ImGuiStyleVar_WindowPadding, 0.0f);
	}

	void InspectorPanel::PostWindow() {
		ImGui::PopStyleVar();
	}

	void InspectorPanel::LoadObjects() {
		m_inspectors.clear();

		auto& manager = App::get().getAssemblyManager();
		m_inspectorClass = manager->getScriptCoreAssembly()->CreateClass("Editor", "Inspector");
		m_inspectorAttribute = manager->getScriptCoreAssembly()->CreateClass("Editor", "InspectorRendererAttribute");
		m_renderMethod = m_inspectorClass.GetMethodFromName("Impl_Render", 1);


		LoadScriptObjects(manager->getScriptCoreAssembly());
		LoadScriptObjects(manager->getAppAssembly());
	}
	void InspectorPanel::LoadScriptObjects(Ref<ScriptAssembly> assembly) {
		auto& classes = assembly->LoadAllClasses(m_inspectorClass);

		Mono::ClassField typeField = m_inspectorAttribute.GetFieldFromName("type");
		Mono::ClassField nameField = m_inspectorAttribute.GetFieldFromName("name");

		CORE_ASSERT(typeField, "Field 'type' not found!");
		CORE_ASSERT(nameField, "Field 'name' not found!");

		for (Mono::Class& inspector : classes) {
			Mono::CustomAttrInfo atrrInfo = Mono::CustomAttrInfo::FromClass(inspector);
			if (atrrInfo.HasAttribute(m_inspectorAttribute)) {
				Mono::Object atrributeObject = atrrInfo.GetAttribute(m_inspectorAttribute);
				if (!atrributeObject)
					continue;

				Mono::ReflectionType reftype = nullptr;
				Mono::String header = nullptr;

				typeField.GetValue(atrributeObject, &reftype);
				nameField.GetValue(atrributeObject, &header);
				
				m_inspectors.push_back(std::move(InspectorRenderer(reftype, header.ToUtf8(), inspector, assembly.get())));
			}

			atrrInfo.Free();
		}
	}
}
