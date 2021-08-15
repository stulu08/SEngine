#include "st_pch.h"
#include "StuluImGui.h"

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#ifndef IMGUI_DISABLE

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"

#include <ctype.h>      // toupper
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127)     // condition expression is constant
#pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#if defined(_MSC_VER) && _MSC_VER >= 1922 // MSVC 2019 16.2 or later
#pragma warning (disable: 5054)     // operator '|': deprecated between enumerations of different types
#endif
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'                      // not all warnings are known by all Clang versions and they tend to be rename-happy.. so ignoring warnings triggers new warnings on some configuration. Great!
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wold-style-cast"                 // warning: use of old-style cast                            // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe // storing and comparing against same constants (typically 0.0f) is ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"              // warning: format string is not a string literal            // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wsign-conversion"                // warning: implicit conversion changes signedness
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning: zero as null pointer constant                    // some standard header variations use #define NULL 0
#pragma clang diagnostic ignored "-Wdouble-promotion"               // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#pragma clang diagnostic ignored "-Wenum-enum-conversion"           // warning: bitwise operation between different enumeration types ('XXXFlags_' and 'XXXFlagsPrivate_')
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"// warning: bitwise operation between different enumeration types ('XXXFlags_' and 'XXXFlagsPrivate_') is deprecated
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"  // warning: implicit conversion from 'xxx' to 'float' may lose precision
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wformat-nonliteral"        // warning: format not a string literal, format string not checked
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

//-------------------------------------------------------------------------
// Data
//-------------------------------------------------------------------------

// Those MIN/MAX values are not define because we need to point to them
static const signed char    IM_S8_MIN = -128;
static const signed char    IM_S8_MAX = 127;
static const unsigned char  IM_U8_MIN = 0;
static const unsigned char  IM_U8_MAX = 0xFF;
static const signed short   IM_S16_MIN = -32768;
static const signed short   IM_S16_MAX = 32767;
static const unsigned short IM_U16_MIN = 0;
static const unsigned short IM_U16_MAX = 0xFFFF;
static const ImS32          IM_S32_MIN = INT_MIN;    // (-2147483647 - 1), (0x80000000);
static const ImS32          IM_S32_MAX = INT_MAX;    // (2147483647), (0x7FFFFFFF)
static const ImU32          IM_U32_MIN = 0;
static const ImU32          IM_U32_MAX = UINT_MAX;   // (0xFFFFFFFF)
#ifdef LLONG_MIN
static const ImS64          IM_S64_MIN = LLONG_MIN;  // (-9223372036854775807ll - 1ll);
static const ImS64          IM_S64_MAX = LLONG_MAX;  // (9223372036854775807ll);
#else
static const ImS64          IM_S64_MIN = -9223372036854775807LL - 1;
static const ImS64          IM_S64_MAX = 9223372036854775807LL;
#endif
static const ImU64          IM_U64_MIN = 0;
#ifdef ULLONG_MAX
static const ImU64          IM_U64_MAX = ULLONG_MAX; // (0xFFFFFFFFFFFFFFFFull);
#else
static const ImU64          IM_U64_MAX = (2ULL * 9223372036854775807LL + 1);
#endif

//-------------------------------------------------------------------------
// [SECTION] Forward Declarations
//-------------------------------------------------------------------------

// For InputTextEx()
static bool             InputTextFilterCharacter(unsigned int* p_char, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);
static int              InputTextCalcTextLenAndLineCount(const char* text_begin, const char** out_text_end);
static ImVec2           InputTextCalcTextSizeW(const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining = NULL, ImVec2* out_offset = NULL, bool stop_on_new_line = false);
static const ImGuiDataTypeInfo GDataTypeInfo[] =
{
	{ sizeof(char),             "%d",   "%d"    },  // ImGuiDataType_S8
	{ sizeof(unsigned char),    "%u",   "%u"    },
	{ sizeof(short),            "%d",   "%d"    },  // ImGuiDataType_S16
	{ sizeof(unsigned short),   "%u",   "%u"    },
	{ sizeof(int),              "%d",   "%d"    },  // ImGuiDataType_S32
	{ sizeof(unsigned int),     "%u",   "%u"    },
#ifdef _MSC_VER
	{ sizeof(ImS64),            "%I64d","%I64d" },  // ImGuiDataType_S64
	{ sizeof(ImU64),            "%I64u","%I64u" },
#else
	{ sizeof(ImS64),            "%lld", "%lld"  },  // ImGuiDataType_S64
	{ sizeof(ImU64),            "%llu", "%llu"  },
#endif
	{ sizeof(float),            "%f",   "%f"    },  // ImGuiDataType_Float (float are promoted to double in va_arg)
	{ sizeof(double),           "%f",   "%lf"   },  // ImGuiDataType_Double
};
IM_STATIC_ASSERT(IM_ARRAYSIZE(GDataTypeInfo) == ImGuiDataType_COUNT);

using namespace ImGui;
namespace Stulu {
	bool imGui::Transform(Stulu::Transform& transform, std::string header, bool _2D, bool end, bool begin) {
		ST_PROFILING_FUNCTION();
		bool returnValue = false;

		if (begin)
			ImGui::Begin(header.c_str());

		ImGui::Text(header.c_str());
		if (_2D) {

			ImGui::Text("Position");
			ImGui::SameLine();
			returnValue = DragScalarFloat(std::string("Position_2d_" + header).c_str(), glm::value_ptr(transform.position), 2, .1f, 0, 0, "%.3f");
			

			ImGui::Text("Size    ");
			ImGui::SameLine();
			returnValue = DragScalarFloat(std::string("Size_2d_" + header).c_str(), glm::value_ptr(transform.scale), 2, .1f, 0, 0, "%.3f");
			

			ImGui::Text("Rotation");
			ImGui::SameLine();
			returnValue = DragScalarFloat(std::string("Rotation_2d_" + header).c_str(), &transform.rotation.z, 1, .1f, 0, 0, "%.3f");
		}
		else{

			ImGui::Text("Position");
			ImGui::SameLine();
			returnValue = DragScalarFloat(std::string("Position_3d_" + header).c_str(), glm::value_ptr(transform.position), 3, .1f, 0, 0, "%.3f");


			ImGui::Text("Rotation");
			ImGui::SameLine();
			returnValue = DragScalarFloat(std::string("Rotation_3d_" + header).c_str(), glm::value_ptr(transform.rotation), 3, .1f, 0, 0, "%.3f");


			ImGui::Text("Scale   ");
			ImGui::SameLine();
			returnValue = DragScalarFloat(std::string("Scale_3d_" + header).c_str(), glm::value_ptr(transform.scale), 3, .1f, 0, 0, "%.3f");
		}
		if (ImGui::Button(std::string("Reset Transform " + header).c_str())) {
			transform.reset();
			returnValue = true;
		}
		if (end)
			ImGui::End();
		return returnValue;
	}
	bool imGui::SliderFloat(const char* header, float* v, float min, float max, const char* format, float scaleFactor, float minWidth)
	{
		ST_PROFILING_FUNCTION();
		ImGui::Text(header);
		ImGui::SameLine();
		return SliderScalarFloat(header, v, 1, &min, &max, format, scaleFactor, minWidth);
	}
	bool imGui::DragScalarFloat(const char* label, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, float scaleFactor, float minWidth)
	{
		ST_PROFILING_FUNCTION();
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;
		ImGuiContext& g = *GImGui;
		bool value_changed = false;
		BeginGroup();
		PushID(label);
		PushMultiItemsWidths(components, std::max(CalcItemWidth() / scaleFactor, minWidth));
		size_t type_size = GDataTypeInfo[ImGuiDataType_Float].Size;
		for (int i = 0; i < components; i++)
		{
			PushID(i);
			if (i > 0)
				SameLine(0, g.Style.ItemInnerSpacing.x);
			value_changed |= DragScalar("", ImGuiDataType_Float, p_data, v_speed, p_min, p_max, format, 0);
			PopID();
			PopItemWidth();
			p_data = (void*)((char*)p_data + type_size);
		}
		PopID();



		EndGroup();
		return value_changed;
	}
	bool imGui::SliderScalarFloat(const char* label, void* v, int components, const void* v_min, const void* v_max, const char* format, float scaleFactor, float minWidth)
	{
		ST_PROFILING_FUNCTION();
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		bool value_changed = false;
		BeginGroup();
		PushID(label);
		PushMultiItemsWidths(components, std::max(CalcItemWidth() / scaleFactor, minWidth));
		size_t type_size = GDataTypeInfo[ImGuiDataType_Float].Size;
		for (int i = 0; i < components; i++)
		{
			PushID(i);
			if (i > 0)
				SameLine(0, g.Style.ItemInnerSpacing.x);
			value_changed |= SliderScalar("", ImGuiDataType_Float, v, v_min, v_max, format, 0);
			PopID();
			PopItemWidth();
			v = (void*)((char*)v + type_size);
		}
		PopID();

		EndGroup();
		return value_changed;
	}
}
#endif // #ifndef IMGUI_DISABLE