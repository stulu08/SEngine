#pragma once
#include "Stulu/App.h"

namespace StuluBindings {
	class Editor {
	public:
		inline static bool IsRuntime() {
			return ::Editor::App::get().GetLayer().IsRuntime();
		}
	};
}