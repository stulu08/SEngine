#pragma once
#include "Editor/Panel/ComponentsRender.h"
#include <mono/metadata/object.h>
#include <imgui/imgui_internal.h>

namespace StuluBindings::Editor {
	class Gui {
	public:
		static inline int32_t Begin(MonoString* str) {
			ST_PROFILING_FUNCTION();
			if (ImGui::Begin(mono_string_to_utf8(str)))
				return 1;
			else
				return 0;
		}
		static inline void End() {
			ST_PROFILING_FUNCTION();
			ImGui::End();
		}
		static inline void Text(MonoString* str) {
			ImGui::Text(mono_string_to_utf8(str));
		}
		static inline int32_t CheckBox(MonoString* str, int32_t checked) {
			ST_PROFILING_FUNCTION();
			bool b = checked == 0 ? false : true;
			Stulu::ComponentsRender::drawBoolControl(mono_string_to_utf8(str), b);
			return b ? 1 : 0;
		}
		static inline int32_t SliderInt(MonoString* str, int32_t value, int32_t min, int32_t max) {
			ST_PROFILING_FUNCTION();
			int b = value;
			Stulu::ComponentsRender::drawIntSliderControl(mono_string_to_utf8(str), b, min, max);
			return b;
		}
		static inline int32_t Int(MonoString* str, int32_t value) {
			ST_PROFILING_FUNCTION();
			int b = value;
			Stulu::ComponentsRender::drawIntControl(mono_string_to_utf8(str), b);
			return b;
		}

		static inline float SliderFloat(MonoString* str, float value, float min, float max) {
			ST_PROFILING_FUNCTION();
			float b = value;
			Stulu::ComponentsRender::drawFloatSliderControl(mono_string_to_utf8(str), b, min, max);
			return b;
		}
		static inline float Float(MonoString* str, float value) {
			ST_PROFILING_FUNCTION();
			float b = value;
			Stulu::ComponentsRender::drawFloatControl(mono_string_to_utf8(str), b);
			return b;
		}
		struct vector3 { float x, y, z; };
		static inline int Vector3(MonoString* str, struct vector3* vec) {
			/*glm::vec3 t = {(vec)->x,(vec)->y,(vec)->z};
			Stulu::ComponentsRender::drawVector3Control(mono_string_to_utf8(str), t);
			return vector3{t.x,t.y,t.z};
			*/
			ST_PROFILING_FUNCTION();
			ImGui::PushID(mono_string_to_utf8(str));
			bool change = false;
			ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text(mono_string_to_utf8(str));
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

			change |= ImGui::DragFloat("##X", &vec->x, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::SameLine();
			change |= ImGui::DragFloat("##Y", &vec->y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::SameLine();
			change |= ImGui::DragFloat("##Z", &vec->z, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::EndColumns();

			ImGui::PopID();
			return change ? 1 : 0;
		}
	};
}