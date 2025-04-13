#pragma once
#include "Stulu/App.h"
#include "Stulu/MainLayer.h"
#include "Stulu/Panels/Inspector.h"

namespace StuluBindings {
	class Editor {
	public:
		inline static bool IsRuntime() {
			return ::Editor::App::get().GetLayer().IsRuntime();
		}
		inline static bool TypeHasInspector(Stulu::Mono::ReflectionType reftype) {
			const auto& panel = ::Editor::App::get().GetLayer().GetPanel<::Editor::InspectorPanel>();
			return panel.HasInspector(reftype);
		}
	};
}