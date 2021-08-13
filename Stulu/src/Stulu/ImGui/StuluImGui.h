#pragma once
#include <Stulu/Renderer/Transform.h>


namespace Stulu {

	class imGui {
	public:
		static bool Transform(Stulu::Transform& transform, std::string header, bool _2D = false, bool end = true, bool begin = true);
		static bool SliderFloat(const char* header, float* v, float min = 0.0f, float max = 1.0f, const char* format = "%.2f", float scaleFactor = 2.0f, float minWidth = 175.0f);

		static bool DragScalarFloat(const char* label, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, float scaleFactor = 2.0f, float minWidth = 175.0f);
		static bool imGui::SliderScalarFloat(const char* label, void* v, int components, const void* v_min, const void* v_max, const char* format, float scaleFactor = 2.0f, float minWidth = 175.0f);
	};
}
