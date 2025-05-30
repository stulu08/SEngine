#pragma once

#include "Controls/BaseControls.h"
#include "Controls/PrimitivControls.h"
#include "Controls/VectorControls.h"
#include "Controls/ColorControl.h"
#include "Controls/EngineControls.h"
#include "Controls/DragDrop.h"
#include "Icons.h"

namespace Editor {
	namespace Controls {
		template<class T>
		inline bool Default(const std::string& label, T& value) {
			static_assert(false, "Default Controle not defined for T!");
			return false;
		}

		template<>
		inline bool Default(const std::string& label, int32_t& value) {
			return Controls::Int(label, value);
		}
		template<>
		inline bool Default(const std::string& label, uint32_t& value) {
			return Controls::UInt(label, value);
		}
		template<>
		inline bool Default(const std::string& label, float& value) {
			return Controls::Float(label, value);
		}
		template<>
		inline bool Default(const std::string& label, bool& value) {
			return Controls::Bool(label, value);
		}
		template<>
		inline bool Default(const std::string& label, glm::vec2& value) {
			return Controls::Vector2(label, value);
		}
		template<>
		inline bool Default(const std::string& label, glm::vec3& value) {
			return Controls::Vector3(label, value);
		}
		template<>
		inline bool Default(const std::string& label, glm::vec4& value) {
			return Controls::Vector4(label, value);
		}
	}
}