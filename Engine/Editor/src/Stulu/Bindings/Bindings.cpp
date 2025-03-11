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
	}
	void LoadImGuiBindings() {
		Ref<AssemblyManager> manager = App::get().getAssemblyManager();

		manager->RegisterFunction("Editor.EditorCalls::ImGui_Vector3(string,Stulu.Vector3&,single)", StuluBindings::ImGui::Vector3);

	}
}
