#pragma once
#include <Stulu.h>
#include <imgui/imgui.h>
namespace Stulu {
	class ComponentsRender {
	public:
		template<typename T>
		static void drawComponent(GameObject gameObject, T& component);
	};
}