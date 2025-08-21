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
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Vector2(string,Stulu.Vector2&,single)", StuluBindings::ImGui::Vector2);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Vector3(string,Stulu.Vector3&,single)", StuluBindings::ImGui::Vector3);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Float(string,single&,single,single,single)", StuluBindings::ImGui::Float);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Int(string,int&,int,int,single)", StuluBindings::ImGui::Int);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_UInt(string,uint&,uint,uint,single)", StuluBindings::ImGui::UInt);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Bool(string,bool&)", StuluBindings::ImGui::Bool);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_TreeNode(string,uint)", StuluBindings::ImGui::TreeBegin);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_TreePop()", ImGui::TreePop);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_InspectorTreeNodeIndent(string,bool&,string)", StuluBindings::ImGui::InspectorTreeBeginIndent);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_InspectorTreePopUnindent()", StuluBindings::ImGui::InspectorTreePopunindent);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Combo(string,int&,string)", StuluBindings::ImGui::Combo);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Button(string)", StuluBindings::ImGui::Button);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_PushID(string)", StuluBindings::ImGui::PushID);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_PopID()", ImGui::PopID);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Texture2D(string,ulong&)", StuluBindings::ImGui::Texture2D);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Material(string,ulong&)", StuluBindings::ImGui::Material);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_Mesh(string,ulong&)", StuluBindings::ImGui::Mesh);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_GameObject(string,ulong&)", StuluBindings::ImGui::GameObject);
		manager->RegisterFunction("Editor.EditorCalls::ImGui_SameLine(single,single)", ImGui::SameLine);
		manager->RegisterFunction("Editor.EditorCalls::Indent(single)", ImGui::Indent);
		manager->RegisterFunction("Editor.EditorCalls::Unindent(single)", ImGui::Unindent);
		
	}
}
