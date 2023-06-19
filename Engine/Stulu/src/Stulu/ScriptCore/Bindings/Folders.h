#pragma once
#include "Stulu/Core/Resources.h"
#include <mono/metadata/object.h>

namespace StuluBindings {
	class Folders {
	public:
		static inline MonoString* assetsPath() {
			return mono_string_new(getCoreDomain(), Stulu::Resources::GameAssetDirectory.c_str());
		}
	};
}