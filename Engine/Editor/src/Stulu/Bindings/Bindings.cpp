#include "Bindings.h"
#include "Stulu/App.h"

#include <Stulu/Scripting/Managed/AssemblyManager.h>

#include "Editor.h"
#include "ImGui.h"

using namespace Stulu;

namespace Editor {
	void LoadEditorBindings() {
		Ref<AssemblyManager> manager = App::get().getAssemblyManager();

		manager->RegisterFunction("Editor.EditorCalls::Editor_IsRuntime()", StuluBindings::Editor::IsRuntime);
		manager->RegisterFunction("Editor.EditorCalls::Editor_TypeHasInspector(System.Type)", StuluBindings::Editor::TypeHasInspector);
	}
	void LoadImGuiBindings() {
		Ref<AssemblyManager> manager = App::get().getAssemblyManager();

		manager->RegisterFunction("Editor.EditorCalls::ImGui_Text(string)", StuluBindings::ImGui::Text);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Vector3(string,Stulu.Vector3&,single)", StuluBindings::ImGui::Vector3);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Float(string,single&,single,single,single)", StuluBindings::ImGui::Float);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Bool(string,bool&)", StuluBindings::ImGui::Bool);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_TreeNode(string,uint)", StuluBindings::ImGui::TreeBegin);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_TreePop()", ImGui::TreePop);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Combo(string,int&,string)", StuluBindings::ImGui::Combo);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_PushID(string)", StuluBindings::ImGui::PushID);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_PopID()", ImGui::PopID);

	}
}
