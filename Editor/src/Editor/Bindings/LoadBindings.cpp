#include "st_pch.h"
#include "mono/jit/jit.h"

#include "Editor/Bindings/Gui.h"

namespace Stulu {
#define StuluUIFuncTypeRegister(TYPE, TypeName) \
	{\
		Stulu::MonoClassMemberTypeFnInfo::infos[TypeName].uiFunc = [](Stulu::Ref<Stulu::MonoObjectInstance>& script, MonoObjectInstance::MonoClassMember& field, const std::string& name) { \
			TYPE& value = *((TYPE*)field.value); \
			if (ComponentsRender::drawControl<TYPE>(name, value)) { \
				script->getFields()[name].value = (void*)&value; \
				script->setClassField(name); \
			} \
		}; \
	} \


	void loadEditorMonoBindings() {
		ST_INFO("Loading Editor Bindings");

		mono_add_internal_call("Stulu.Editor.InternalCalls::gui_Begin(string)", StuluBindings::Editor::Gui::Begin);
		mono_add_internal_call("Stulu.Editor.InternalCalls::gui_End()", StuluBindings::Editor::Gui::End);

		mono_add_internal_call("Stulu.Editor.InternalCalls::gui_Text(string)", StuluBindings::Editor::Gui::Text);

		mono_add_internal_call("Stulu.Editor.InternalCalls::gui_CheckBox(string,int)", StuluBindings::Editor::Gui::CheckBox);

		mono_add_internal_call("Stulu.Editor.InternalCalls::gui_SliderInt(string,int,int,int)", StuluBindings::Editor::Gui::SliderInt);
		mono_add_internal_call("Stulu.Editor.InternalCalls::gui_Int(string,int)", StuluBindings::Editor::Gui::Int);

		mono_add_internal_call("Stulu.Editor.InternalCalls::gui_SliderFloat(string,single,single,single)", StuluBindings::Editor::Gui::SliderFloat);
		mono_add_internal_call("Stulu.Editor.InternalCalls::gui_Float(string,single)", StuluBindings::Editor::Gui::Float);

		mono_add_internal_call("Stulu.Editor.InternalCalls::gui_Vector3(string,Stulu.Vector3&)", StuluBindings::Editor::Gui::Vector3);

		ST_INFO("Finished Loading Editor Bindings");

		StuluUIFuncTypeRegister(glm::vec4, "Stulu.Vector4");
		StuluUIFuncTypeRegister(glm::vec3, "Stulu.Vector4");
		StuluUIFuncTypeRegister(glm::vec2, "Stulu.Vector4");
		StuluUIFuncTypeRegister(float, "System.Single");
		StuluUIFuncTypeRegister(uint32_t, "System.UInt32");
		StuluUIFuncTypeRegister(int32_t, "System.Int32");
		StuluUIFuncTypeRegister(bool, "System.Boolean");
	}
}