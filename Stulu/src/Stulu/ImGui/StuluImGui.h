#pragma once
#include "Stulu/Math/Math.h"

namespace Stulu {

	class imGui {
	public:
		static bool SliderFloat(const char* header, float* v, float min = 0.0f, float max = 1.0f, const char* format = "%.2f", float scaleFactor = 1.0f, float minWidth = .0f);
		static bool Float(const char* header, float& v, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char* format = "%.2f", float scaleFactor = 1.0f, float minWidth = .0f);
		static bool vec2(const char* header, glm::vec2& v, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char* format = "%.2f", float scaleFactor = 1.0f, float minWidth = .0f);
		static bool vec3(const char* header, glm::vec3& v, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char* format = "%.2f", float scaleFactor = 1.0f, float minWidth = .0f);
		static bool vec4(const char* header, glm::vec4& v, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char* format = "%.2f", float scaleFactor = 1.0f, float minWidth = .0f);
		static bool checkBox(const char* header, bool& v, bool headerText = true);

		static bool DragScalarFloatNoLabel(const char* id, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, float scaleFactor = 1.0f, float minWidth = .0f);
		static bool SliderScalarFloatNoLabel(const char* id, void* v, int components, const void* v_min, const void* v_max, const char* format, float scaleFactor = 1.0f, float minWidth = .0f);
	};
}
