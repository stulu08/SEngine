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
		static inline bool Float(Stulu::Mono::String monoName, float* value, float min, float max, float speed) {
			return ::Editor::Controls::Float(monoName.ToUtf8(), *value, min, max, speed);
		}
		static inline bool Bool(Stulu::Mono::String monoName, bool* value) {
			return ::Editor::Controls::Bool(monoName.ToUtf8(), *value);
		}
	};
}