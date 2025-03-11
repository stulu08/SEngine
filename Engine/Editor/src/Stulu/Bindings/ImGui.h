#pragma once
#include "Stulu/Controls.h"
#include <Stulu/Scripting/Managed/Bindings/Bindings.h>

namespace StuluBindings {
	class ImGui {
	public:
		static inline bool Vector3(Stulu::Mono::String monoName, struct Vector3* value, float speed) {
			glm::vec3* valuePtr = (glm::vec3*)value;
			return ::Editor::Controls::Vector3(monoName.ToUtf8(), *valuePtr, speed);
		}
	};
}