#pragma once
#include "../Resources.h"
#include <imgui/imgui.h>
namespace Stulu {
	class ComponentsRender {
	public:
		template<typename T>
		static void drawComponent(GameObject gameObject, T& component);

		static bool drawTextureEdit(Ref<Texture2D>& texture, const std::string& header);
	};
}