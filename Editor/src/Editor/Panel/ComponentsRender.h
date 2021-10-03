#pragma once
#include "../Resources.h"
#include <imgui/imgui.h>
namespace Stulu {
	class ComponentsRender {
	public:
		template<typename T>
		static void drawComponent(GameObject gameObject, T& component);

		static bool drawStringControl(const std::string& header, std::string& v);
		static bool drawBoolControl(const std::string& header, bool& v);
		static bool drawIntControl(const std::string& header, int& v);
		static bool drawFloatControl(const std::string& header, float& v);
		static bool drawVector2Control(const std::string& header, glm::vec2& vec);
		static bool drawVector3Control(const std::string& header, glm::vec3& vec);
		static bool drawTextureEdit(const std::string& header, Ref<Texture2D>& texture);
	};
}